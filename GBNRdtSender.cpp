#include "stdafx.h"
#ifndef NONE_GBN_RDT_SENDER_MODE
#include "Global.h"
#include "GBNRdtSender.h"

	GBNRdtSender::GBNRdtSender() :nextseqnum(0), waitingState(false)
	{
	}


	GBNRdtSender::~GBNRdtSender()
	{
	}



	bool GBNRdtSender::getWaitingState() {
		return waitingState;
	}

	int GBNRdtSender::windowSize = 4;
	//在GBNRdtSender的定义之外定义static数据类型，规范窗口的大小，窗口中的报文用queue进行管理，并于windowSize进行比较

	bool GBNRdtSender::send(const Message& message) {
		if (this->nextseqnum>=this->base+this->windowSize) { //发送方处于等待确认状态
			return false;
		}
		else {
			const Packet& insertPacket = Packet::Packet();
			this->packetWindow.push(insertPacket);
			this->packetWindow.back().acknum = -1; //忽略该字段
			this->packetWindow.back().seqnum = this->nextseqnum;
			this->packetWindow.back().checksum = 0;
			memcpy(this->packetWindow.back().payload, message.data, sizeof(message.data));
			this->packetWindow.back().checksum = pUtils->calculateCheckSum(this->packetWindow.back());
			pUtils->printPacket("发送方发送报文", this->packetWindow.back());
			pns->sendToNetworkLayer(RECEIVER, this->packetWindow.back());								//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方

			if (this->base==this->nextseqnum)
				pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWindow.front().seqnum);	//以第一个元素为基准设置超时			//启动发送方定时器

			this->nextseqnum = (this->nextseqnum++) % 8; //接收序号在0-7之间切换																			//进入等待状态
			return true;
		}
	}

	void GBNRdtSender::receive(const Packet& ackPkt) {
		//检查校验和是否正确
		int checkSum = pUtils->calculateCheckSum(ackPkt);

		//如果校验和正确，并且确认序号=发送方已发送并等待确认的数据包序号
		if (checkSum == ackPkt.checksum) {
			this->packetWindow.pop();
			this->base = ackPkt.acknum + 1;
			if(this->base==this->nextseqnum) {
				pUtils->printPacket("发送方正确收到确认", ackPkt);
				pns->stopTimer(SENDER, ackPkt.seqnum);		//关闭定时器
			}
			else {
				pns->stopTimer(SENDER, ackPkt.seqnum);										//首先关闭定时器
				pns->startTimer(SENDER, Configuration::TIME_OUT, ackPkt.seqnum);			//重新启动发送方定时器
			}
		}
		else {}	//GBN中如果包出现了错误就什么也不做
	}

	void GBNRdtSender::timeoutHandler(int seqNum) {
		//在GBN中,整个滑动窗口有唯一一个定时器,无需考虑seqNum
		cout << "发送方定时器时间到，重发窗口内所有的报文" << endl;
		queue <Packet> vicePacketWindow = this->packetWindow;
		for(;vicePacketWindow.size()!=0;vicePacketWindow.pop())
			pUtils->printPacket("报文：", vicePacketWindow.front());

		pns->stopTimer(SENDER, seqNum);										//首先关闭定时器
		pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//重新启动发送方定时器
	
		vicePacketWindow = this->packetWindow;
		for (; vicePacketWindow.size() != 0; vicePacketWindow.pop())
			pns->sendToNetworkLayer(RECEIVER, vicePacketWindow.front());			//重新发送窗口中的所有数据包

	}
#endif
