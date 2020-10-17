#include "stdafx.h"
#ifndef NONE_SR_RDT_SENDER_MODE
	#include "Global.h"
	#include "SRRdtSender.h"

	SRRdtSender::SRRdtSender() :nextSequenceNumber(0), waitingState(false), base(0)
	/*在初始化时，waitingState必然是false，因为sender一开始是能发送的*/
	{
	}

	SRRdtSender::~SRRdtSender()
	{
	}

	void SRRdtSender::printSlideWindow()
	{
		cout << "[[[[[[[[[滑动窗口移动]]]]]]]]]]" << endl;
		int i;
		for (i = 0; i<this->packetWindow.size(); i++)
		{
			printf("滑动窗口第%d个，", i);
			printf("该报文接收情况为：%d " ,this->packetWindow[i].ackStatus);	//针对SR，打印滑动窗口的同时还要打印滑动窗口中每个报文的接收状态
			pUtils->printPacket("报文：", this->packetWindow[i].realPacket);
		}
		while (i != this->windowSize)
		{
			printf("滑动窗口第%d个为空且可用\n", i);
			i++;
		}
		cout << "[[[[[[[[[滑动窗口打印结束]]]]]]]]]]" << endl;
		return;
	}

	bool SRRdtSender::inPacketWindow(int seqnum) {
		bool result = false;
		int i = 0;
		for (; i < this->packetWindow.size() && this->packetWindow[i].realPacket.seqnum != seqnum; i++);
		if (i<this->packetWindow.size()&&this->packetWindow[i].realPacket.seqnum==seqnum)		//如果找到了匹配的发送报文序列号
		/*还要确保i不能超过size造成越界*/
			if (this->packetWindow[i].ackStatus != true) {	//还要确保从前没有收到过同样的确认，以免计时器重复关闭等错误的出现
				this->packetWindow[i].ackStatus = true;
				result = true;
				/*不要忘记修改result*/
			}
		return result;
	}

	bool SRRdtSender::getWaitingState() {

		if (this->packetWindow.size() == this->windowSize)
			/*如果使用简单的大小关系比较，则与模运算的序号冲突*/
			return true;
		else
			return false;
		/*上层应用层调用先看waitingstate是否为true，针对SR，如果窗口满了，相当于处在等待状态，因此返回true，应用层收到后等待下一次发送；反之则返回false
		看似更为高明的应该是在下面对queue进行操作时实时判断是否窗口满，如果窗口满了就将类中的waitingState变量进行设置，而本函数只需要返回waitingState就可以
		但这样会增大代码量和时间复杂度，还不如更为简洁的本函数*/
	}

	int SRRdtSender::windowSize = 4;
	//在SRRdtSender的定义之外定义static数据类型，规范窗口的大小，窗口中的报文用queue进行管理，并于windowSize进行比较
	//其实用非静态成员变量也可以，这里只是多一种尝试

	bool SRRdtSender::send(const Message& message) {
		if (this->packetWindow.size() == this->windowSize) { //发送方的窗口已满，拒绝发送
		/*如果使用简单的大小关系比较，则与模运算的序号冲突*/
			return false;
		}
		else {
			const SRPacket& insertPacket = SRPacket::SRPacket();
			this->packetWindow.push_back(insertPacket);
			this->packetWindow.back().realPacket.acknum = -1; //忽略该字段
			this->packetWindow.back().realPacket.seqnum = this->nextSequenceNumber;
			this->packetWindow.back().realPacket.checksum = 0;
			memcpy(this->packetWindow.back().realPacket.payload, message.data, sizeof(message.data));
			this->packetWindow.back().realPacket.checksum = pUtils->calculateCheckSum(this->packetWindow.back().realPacket);
			pUtils->printPacket("发送方发送报文", this->packetWindow.back().realPacket);
			pns->sendToNetworkLayer(RECEIVER, this->packetWindow.back().realPacket);								//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方
			this->printSlideWindow();
			/*无条件重新启动定时器！*/
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWindow.back().realPacket.seqnum);	//只对刚发送的报文对应的序号设置超时，启动发送方定时器

			this->nextSequenceNumber = (++this->nextSequenceNumber) % 8; //接收序号在0-7之间切换
			/*++千万别放到后面！*/
			/*如果序号数和窗口大小相同，则可能会因为确认报文以窗口大小为单位丢失，而引起数据报文以窗口大小为单位重复传输*/
			return true;
		}
	}

	void SRRdtSender::receive(const Packet& ackPkt) {
		//检查校验和是否正确
		int checkSum = pUtils->calculateCheckSum(ackPkt);

		//如果校验和正确就调整base和计时器
		/*并且，只有在确认序号=发送方已发送并等待确认的数据包序号才出队列！*/
		if (checkSum == ackPkt.checksum) {
			{
				pUtils->printPacket("发送方正确收到确认", ackPkt);
				if (inPacketWindow(ackPkt.acknum))
					//如果确认报文的序列号在等待队列中，就停止计时器，并视情况移动滑动窗口
				{
					pns->stopTimer(SENDER, ackPkt.acknum);		//关闭ACK报文对应的定时器
				/*注意ACK报文的序列号来自包的acknum成员*/
					while (this->packetWindow.size() != 0 && this->packetWindow.front().ackStatus == true)
						//移动滑动窗口直到第一个未收到确认的成员或窗口中未收到确认成员为空
					{
						this->packetWindow.erase(this->packetWindow.begin());
						this->printSlideWindow();
						this->base = (++this->base) % 8;
					}
				}
				//由于每个已发送报文都有单独的计时器，因此不用再像GBN一样判断是否重启计时器
				else cout << "发送方收到的确认是老的确认/已经得到的确认" << endl;
			}
		}
		else
			pUtils->printPacket("发送方收到的包校验和错误", ackPkt);//SR中如果包在校验上出现了错误就单纯输出提示，什么也不做
	}

	void SRRdtSender::timeoutHandler(int seqNum) {
		//在SR中,每个发送但没有收到确认的报文有唯一的定时器,需要考虑seqNum
		cout << "发送方对应报文定时器时间到，重新发送这份超时的报文" << endl;
		int i;
		for (i = 0; this->packetWindow[i].realPacket.seqnum != seqNum; i++);
		pUtils->printPacket("报文：", this->packetWindow[i].realPacket);
		/*报文要正确选取！*/

		pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
		pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器

		pns->sendToNetworkLayer(RECEIVER, this->packetWindow[i].realPacket);			//重新发送窗口中超时的这个数据包

	}
#endif
