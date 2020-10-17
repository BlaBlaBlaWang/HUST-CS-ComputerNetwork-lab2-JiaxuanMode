#include "stdafx.h"
#ifndef NONE_TCP_RDT_SENDER_MODE
#include "Global.h"
#include "TCPRdtSender.h"

	TCPRdtSender::TCPRdtSender() :nextSequenceNumber(0), waitingState(false),base(0),surplusAck(0)
	/*在初始化时，waitingState必然是false，因为sender一开始是能发送的*/
	{
	}

	TCPRdtSender::~TCPRdtSender()
	{
	}

	void TCPRdtSender::printSlideWindow()
	{
		cout << "[[[[[[[[[滑动窗口移动]]]]]]]]]]" << endl;
		queue <Packet> vicePacketWindow = this->packetWindow;
		int i;
		for (i = 0; vicePacketWindow.size() != 0; vicePacketWindow.pop(), i++)
		{
			printf("滑动窗口第%d个，", i);
			pUtils->printPacket("报文：", vicePacketWindow.front());
		}
		while (i != this->windowSize)
		{
			printf("滑动窗口第%d个为空且可用\n", i);
			i++;
		}
		cout << "[[[[[[[[[滑动窗口打印结束]]]]]]]]]]" << endl;
		return;
	}

	bool TCPRdtSender::getWaitingState() {
		
		if (this->packetWindow.size() == this->windowSize)
		/*如果使用简单的大小关系比较，则与模运算的序号冲突*/
			return true;
		else
			return false;
	/*上层应用层调用先看waitingstate是否为true，针对GBN，如果窗口满了，相当于处在等待状态，因此返回true，应用层收到后等待下一次发送；反之则返回false
	看似更为高明的应该是在下面对queue进行操作时实时判断是否窗口满，如果窗口满了就将类中的waitingState变量进行设置，而本函数只需要返回waitingState就可以
	但这样会增大代码量和时间复杂度，还不如更为简洁的本函数*/
	}
	
	int TCPRdtSender::windowSize = 4;
	//在GBNRdtSender的定义之外定义static数据类型，规范窗口的大小，窗口中的报文用queue进行管理，并于windowSize进行比较
	//其实用非静态成员变量也可以，这里只是多一种尝试

	bool TCPRdtSender::send(const Message& message) {
		if (this->packetWindow.size()==this->windowSize) { //发送方的窗口已满，拒绝发送
		/*如果使用简单的大小关系比较，则与模运算的序号冲突*/
			return false;
		}
		else {
			const Packet& insertPacket = Packet::Packet();
			this->packetWindow.push(insertPacket);
			this->packetWindow.back().acknum = -1; //忽略该字段
			this->packetWindow.back().seqnum = this->nextSequenceNumber;
			this->packetWindow.back().checksum = 0;
			memcpy(this->packetWindow.back().payload, message.data, sizeof(message.data));
			this->packetWindow.back().checksum = pUtils->calculateCheckSum(this->packetWindow.back());
			pUtils->printPacket("发送方发送报文", this->packetWindow.back());
			pns->sendToNetworkLayer(RECEIVER, this->packetWindow.back());								//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
			this->printSlideWindow();
			if (this->base==this->nextSequenceNumber)
				pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);	//以第一个元素为基准设置超时，启动发送方定时器

			this->nextSequenceNumber = (++this->nextSequenceNumber) % 8; //接收序号在0-7之间切换
			/*++千万别放到后面！*/
			/*如果序号数和窗口大小相同，则可能会因为确认报文以窗口大小为单位丢失，而引起数据报文以窗口大小为单位重复传输*/
			return true;
		}
	}

	void TCPRdtSender::receive(const Packet& ackPkt) {
		//检查校验和是否正确
		int checkSum = pUtils->calculateCheckSum(ackPkt);

		//如果校验和正确就调整base和计时器
		/*并且，只有在确认序号=发送方已发送并等待确认的数据包序号才出队列！*/
		if (checkSum == ackPkt.checksum) {
			{
				pUtils->printPacket("发送方正确收到确认", ackPkt);
				if (this->base != ackPkt.seqnum)	//如果收到的下一期望序号和当前的base不同，说明窗口内报文成功传送到接收方，窗口可以移动
				{
					this->surplusAck = 0;	//窗口移动，将冗余ACK数归零
					pns->stopTimer(SENDER, this->base);		//关闭定时器
					/*注意ACK报文的序列号来自包的acknum成员*/
					this->base = ackPkt.seqnum;
					/*base的更新应当参照自顶向下方法的TCP部分，这里的seqnum是下一个期待的序号*/
					if (this->packetWindow.size() != 0 && this->packetWindow.front().seqnum != this->base) {
						while (this->packetWindow.size() != 0 && this->packetWindow.front().seqnum != this->base)
							this->packetWindow.pop();
						this->printSlideWindow();
					}
					/*窗口队列出队直到第一个的序号与新的base相同*/
					if (this->packetWindow.size() != 0)
						/*由于牵扯到模数，因此不光要看nextSequenceNum和base两者是否相等来决定是否重启重传，还要看窗口的大小是否为0，准确的说一个窗口大小为0即可*/
						pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);				//如果窗口中还有待确认的报文，就重新启动发送方定时器
						/*注意重启后定时器的序号来自发送方窗口中的最前项，也就是base指定的*/
				}
				else
				{
					this->surplusAck++;
					if (this->surplusAck == 3)	//如果冗余ACK数加到3，就重传窗口内的全部报文
					{
						cout << "发送方收到三个冗余ACK，重发窗口内所有的报文" << endl;
						queue <Packet> vicePacketWindow = this->packetWindow;
						for (; vicePacketWindow.size() != 0; vicePacketWindow.pop())
							pUtils->printPacket("报文：", vicePacketWindow.front());

						pns->stopTimer(SENDER, this->base);										//首先关闭定时器
						pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);			//重新启动发送方定时器

						vicePacketWindow = this->packetWindow;
						for (; vicePacketWindow.size() != 0; vicePacketWindow.pop())
							pns->sendToNetworkLayer(RECEIVER, vicePacketWindow.front());			//重新发送窗口中的所有数据包
						
						this->surplusAck = 0;	//全部重传后，将冗余ACK的数量归0
					}
				}
			}
		}
		else {}	//GBN中如果包在校验上出现了错误就什么也不做
	}

	void TCPRdtSender::timeoutHandler(int seqNum) {
		//在TCP中,整个滑动窗口有唯一一个定时器,无需考虑seqNum
		//和GBN不同，TCP只需要重传base序号对应的的报文就可以
		cout << "发送方定时器时间到，重发窗口内的base报文" << endl;
		pUtils->printPacket("报文：", this->packetWindow.front());

		pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
		pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
	
		pns->sendToNetworkLayer(RECEIVER, this->packetWindow.front());			//重新发送窗口中的base对应数据包

	}
#endif
