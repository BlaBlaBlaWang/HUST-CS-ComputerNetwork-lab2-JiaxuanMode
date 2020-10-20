#include "stdafx.h"
#ifndef NONE_SR_RDT_SENDER_MODE	//调整了预编译相关之后，加入这行，否则会因为包含了已经不编译的头文件带来错误
	#include "Global.h"
	#include "SRRdtReceiver.h"

	SRRdtReceiver::SRRdtReceiver() :base(0)
	{
	}

	int SRRdtReceiver::windowSize = 4;


	void SRRdtReceiver::printSlideWindow()
	{
		cout << "]]]]]]]]]接收方滑动窗口移动[[[[[[[[[[" << endl;
		int i;
		for (i = 0; i < this->packetWindow.size(); i++)
		{
			printf("滑动窗口第%d个，", i);	//针对SR的接收方，打印滑动窗口的同时不需要打印滑动窗口中每个报文的接收状态，因为他们都是缓存的报文
			pUtils->printPacket("报文：", this->packetWindow[i]);
		}
		while (i != this->windowSize)
		{
			printf("滑动窗口第%d个为空且可用\n", i);
			i++;
		}
		cout << "]]]]]]]]]接收方滑动窗口打印结束[[[[[[[[[" << endl;
		return;
	}

	bool SRRdtReceiver::seqNumNew(int seqnum) {
		bool result = false;
		for (int i = 0; i < this->windowSize; i++)
			if ((seqnum ) % 8 == (this->base + i) % 8) {
			/*只有base需要加上i再取模*/
				result = true;
				break;
			}
		return result;
	}

	int SRRdtReceiver::modOrder(int seqnum) {
		if (seqnum - this->base >=0)
		/*模的减法，只有小于0时才加上模数，等于0 不加！*/
			return seqnum - this->base;
		else return seqnum - this->base + 8;
	}

	void SRRdtReceiver::savePktInOrder(Packet packet) {
		int status = 0;	//表示之前已经存放过，或者已经比较完完成插入
		for (int i = 0; i < this->packetWindow.size(); i++)
		{
			if (this->modOrder(this->packetWindow[i].seqnum) == this->modOrder(packet.seqnum))	//已经正确收到过，不需要再缓存
			{
				status = 1;
				break;
			}
			else if (this->modOrder(this->packetWindow[i].seqnum) > this->modOrder(packet.seqnum))	//i脚标处报文seqnum比packet更大，将packet插入缓存到脚标i的位置
			{
				this->packetWindow.insert(this->packetWindow.begin() + i, packet);
				this->printSlideWindow();
				status = 1;
				break;
			}
			else continue;
		}

		if (status == 0)	//如果退出循环是因为vector中所有元素都小于它，则将packet放到最后
		/*要先确保vector不为空，不能对空vector使用size()操作*/
		{
			this->packetWindow.push_back(packet);
			this->printSlideWindow();
		}
		
		return;
	}

	SRRdtReceiver::~SRRdtReceiver()
	{
	}


	void SRRdtReceiver::receive(const Packet& packet) {
		//检查校验和是否正确
		int checkSum = pUtils->calculateCheckSum(packet);

		if (checkSum == packet.checksum ) {		//如果校验和正确，就一定会发送确认报文
			pUtils->printPacket("接收方正确收到发送方的报文", packet);

			Packet AckPacket;
			AckPacket.acknum = packet.seqnum; //确认序号等于收到的报文序号
			AckPacket.seqnum = packet.seqnum;
			memcpy(AckPacket.payload, packet.payload, sizeof(packet.payload));
			AckPacket.checksum = pUtils->calculateCheckSum(AckPacket);
			pUtils->printPacket("接收方发送确认报文", AckPacket);
			pns->sendToNetworkLayer(SENDER, AckPacket);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方

			if (seqNumNew(packet.seqnum))	//如果seqnum在窗口中，就判断是否需要缓存或者交送
			{
				this->savePktInOrder(packet);
				if (packet.seqnum == this->base)	//如果序号与base相同，则将packet与所有缓存的packet按序交付应用层
				{
					Message msg;
					
					//顺序交付packetWindow中缓存且连续的
					for (; this->packetWindow.size()!=0 && this->packetWindow[0].seqnum == this->base; )
					{
					/*由于从头开始连续交付并删除，因此访问vector时只访问最前面的*/
					/*这里用size()方法判断是否已经为空，之前使用i自增和size()比较的错误在于，i在变大，size()变小，会交错产生错误*/
						memcpy(msg.data, this->packetWindow[0].payload, sizeof(this->packetWindow[0].payload));
						pns->delivertoAppLayer(RECEIVER, msg);
						this->packetWindow.erase(this->packetWindow.begin());
						//交付后就删除，并让后面的成为vector的头元素
						this->base = (++this->base) % 8;
						/*base修改后一定要取模，而且必须前++，而且不能防在和i相同的自增位*/
						this->printSlideWindow();
					}
				}
			}
		}
		else 
			pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
	}
#endif