#include "stdafx.h"
#ifndef NONE_GBN_RDT_SENDER_MODE
#include "Global.h"
#include "GBNRdtSender.h"

	GBNRdtSender::GBNRdtSender() :nextSequenceNumber(0), waitingState(false),base(0)		
	/*�ڳ�ʼ��ʱ��waitingState��Ȼ��false����Ϊsenderһ��ʼ���ܷ��͵�*/
	{
	}

	GBNRdtSender::~GBNRdtSender()
	{
	}

	void GBNRdtSender::printSlideWindow()
	{
		cout << "[[[[[[[[[���������ƶ�]]]]]]]]]]" << endl;
		queue <Packet> vicePacketWindow = this->packetWindow;
		for (int i = 0; vicePacketWindow.size() != 0; vicePacketWindow.pop(), i++)
		{
			printf("�������ڵ�%d��", i);
			pUtils->printPacket("���ģ�", vicePacketWindow.front());
		}
		cout << "[[[[[[[[[�������ڴ�ӡ����]]]]]]]]]]" << endl;
		return;
	}

	bool GBNRdtSender::getWaitingState() {
		
		if (this->packetWindow.size() == this->windowSize)
		/*���ʹ�ü򵥵Ĵ�С��ϵ�Ƚϣ�����ģ�������ų�ͻ*/
			return true;
		else
			return false;
	/*�ϲ�Ӧ�ò�����ȿ�waitingstate�Ƿ�Ϊtrue�����GBN������������ˣ��൱�ڴ��ڵȴ�״̬����˷���true��Ӧ�ò��յ���ȴ���һ�η��ͣ���֮�򷵻�false
	���Ƹ�Ϊ������Ӧ�����������queue���в���ʱʵʱ�ж��Ƿ񴰿���������������˾ͽ����е�waitingState�����������ã���������ֻ��Ҫ����waitingState�Ϳ���
	�������������������ʱ�临�Ӷȣ��������Ϊ���ı�����*/
	}
	
	int GBNRdtSender::windowSize = 4;
	//��GBNRdtSender�Ķ���֮�ⶨ��static�������ͣ��淶���ڵĴ�С�������еı�����queue���й�������windowSize���бȽ�
	//��ʵ�÷Ǿ�̬��Ա����Ҳ���ԣ�����ֻ�Ƕ�һ�ֳ���

	bool GBNRdtSender::send(const Message& message) {
		if (this->packetWindow.size()==this->windowSize) { //���ͷ��Ĵ����������ܾ�����
		/*���ʹ�ü򵥵Ĵ�С��ϵ�Ƚϣ�����ģ�������ų�ͻ*/
			return false;
		}
		else {
			const Packet& insertPacket = Packet::Packet();
			this->packetWindow.push(insertPacket);
			this->packetWindow.back().acknum = -1; //���Ը��ֶ�
			this->packetWindow.back().seqnum = this->nextSequenceNumber;
			this->packetWindow.back().checksum = 0;
			memcpy(this->packetWindow.back().payload, message.data, sizeof(message.data));
			this->packetWindow.back().checksum = pUtils->calculateCheckSum(this->packetWindow.back());
			pUtils->printPacket("���ͷ����ͱ���", this->packetWindow.back());
			pns->sendToNetworkLayer(RECEIVER, this->packetWindow.back());								//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�
			this->printSlideWindow();
			if (this->base==this->nextSequenceNumber)
				pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);	//�Ե�һ��Ԫ��Ϊ��׼���ó�ʱ���������ͷ���ʱ��

			this->nextSequenceNumber = (++this->nextSequenceNumber) % 8; //���������0-7֮���л�
			/*++ǧ���ŵ����棡*/
			/*���������ʹ��ڴ�С��ͬ������ܻ���Ϊȷ�ϱ����Դ��ڴ�СΪ��λ��ʧ�����������ݱ����Դ��ڴ�СΪ��λ�ظ�����*/
			return true;
		}
	}

	void GBNRdtSender::receive(const Packet& ackPkt) {
		//���У����Ƿ���ȷ
		int checkSum = pUtils->calculateCheckSum(ackPkt);

		//���У�����ȷ�͵���base�ͼ�ʱ��
		/*���ң�ֻ����ȷ�����=���ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ���Ųų����У�*/
		if (checkSum == ackPkt.checksum) {
			{
				pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
				pns->stopTimer(SENDER, this->base);		//�رն�ʱ��
				/*ע��ACK���ĵ����к����԰���acknum��Ա*/
				this->base = (ackPkt.acknum + 1) %8;
				/*base�ĸ���Ӧ�������Զ����·���*/
				if (this->packetWindow.size() != 0 && this->packetWindow.front().seqnum != this->base) {
					while (this->packetWindow.size() != 0 && this->packetWindow.front().seqnum != this->base)
						this->packetWindow.pop();
					this->printSlideWindow();
				}
				/*���ڶ��г���ֱ����һ����������µ�base��ͬ*/
				if(this->packetWindow.size()!=0)
				/*����ǣ����ģ������˲���Ҫ��nextSequenceNum��base�����Ƿ�����������Ƿ������ش�����Ҫ�����ڵĴ�С�Ƿ�Ϊ0��׼ȷ��˵һ�����ڴ�СΪ0����*/
					pns->startTimer(SENDER, Configuration::TIME_OUT, this->base);				//��������л��д�ȷ�ϵı��ģ��������������ͷ���ʱ��
					/*ע��������ʱ����������Է��ͷ������е���ǰ�Ҳ����baseָ����*/
			}
		}
		else {}	//GBN���������У���ϳ����˴����ʲôҲ����
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
