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
	//��GBNRdtSender�Ķ���֮�ⶨ��static�������ͣ��淶���ڵĴ�С�������еı�����queue���й�������windowSize���бȽ�

	bool GBNRdtSender::send(const Message& message) {
		if (this->nextseqnum>=this->base+this->windowSize) { //���ͷ����ڵȴ�ȷ��״̬
			return false;
		}
		else {
			const Packet& insertPacket = Packet::Packet();
			this->packetWindow.push(insertPacket);
			this->packetWindow.back().acknum = -1; //���Ը��ֶ�
			this->packetWindow.back().seqnum = this->nextseqnum;
			this->packetWindow.back().checksum = 0;
			memcpy(this->packetWindow.back().payload, message.data, sizeof(message.data));
			this->packetWindow.back().checksum = pUtils->calculateCheckSum(this->packetWindow.back());
			pUtils->printPacket("���ͷ����ͱ���", this->packetWindow.back());
			pns->sendToNetworkLayer(RECEIVER, this->packetWindow.back());								//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�

			if (this->base==this->nextseqnum)
				pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWindow.front().seqnum);	//�Ե�һ��Ԫ��Ϊ��׼���ó�ʱ			//�������ͷ���ʱ��

			this->nextseqnum = (this->nextseqnum++) % 8; //���������0-7֮���л�																			//����ȴ�״̬
			return true;
		}
	}

	void GBNRdtSender::receive(const Packet& ackPkt) {
		//���У����Ƿ���ȷ
		int checkSum = pUtils->calculateCheckSum(ackPkt);

		//���У�����ȷ������ȷ�����=���ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ����
		if (checkSum == ackPkt.checksum) {
			this->packetWindow.pop();
			this->base = ackPkt.acknum + 1;
			if(this->base==this->nextseqnum) {
				pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
				pns->stopTimer(SENDER, ackPkt.seqnum);		//�رն�ʱ��
			}
			else {
				pns->stopTimer(SENDER, ackPkt.seqnum);										//���ȹرն�ʱ��
				pns->startTimer(SENDER, Configuration::TIME_OUT, ackPkt.seqnum);			//�����������ͷ���ʱ��
			}
		}
		else {}	//GBN������������˴����ʲôҲ����
	}

	void GBNRdtSender::timeoutHandler(int seqNum) {
		//��GBN��,��������������Ψһһ����ʱ��,���迼��seqNum
		cout << "���ͷ���ʱ��ʱ�䵽���ط����������еı���" << endl;
		queue <Packet> vicePacketWindow = this->packetWindow;
		for(;vicePacketWindow.size()!=0;vicePacketWindow.pop())
			pUtils->printPacket("���ģ�", vicePacketWindow.front());

		pns->stopTimer(SENDER, seqNum);										//���ȹرն�ʱ��
		pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//�����������ͷ���ʱ��
	
		vicePacketWindow = this->packetWindow;
		for (; vicePacketWindow.size() != 0; vicePacketWindow.pop())
			pns->sendToNetworkLayer(RECEIVER, vicePacketWindow.front());			//���·��ʹ����е��������ݰ�

	}
#endif
