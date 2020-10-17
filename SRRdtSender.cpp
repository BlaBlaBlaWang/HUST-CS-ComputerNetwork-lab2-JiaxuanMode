#include "stdafx.h"
#ifndef NONE_SR_RDT_SENDER_MODE
	#include "Global.h"
	#include "SRRdtSender.h"

	SRRdtSender::SRRdtSender() :nextSequenceNumber(0), waitingState(false), base(0)
	/*�ڳ�ʼ��ʱ��waitingState��Ȼ��false����Ϊsenderһ��ʼ���ܷ��͵�*/
	{
	}

	SRRdtSender::~SRRdtSender()
	{
	}

	void SRRdtSender::printSlideWindow()
	{
		cout << "[[[[[[[[[���������ƶ�]]]]]]]]]]" << endl;
		int i;
		for (i = 0; i<this->packetWindow.size(); i++)
		{
			printf("�������ڵ�%d����", i);
			printf("�ñ��Ľ������Ϊ��%d " ,this->packetWindow[i].ackStatus);	//���SR����ӡ�������ڵ�ͬʱ��Ҫ��ӡ����������ÿ�����ĵĽ���״̬
			pUtils->printPacket("���ģ�", this->packetWindow[i].realPacket);
		}
		while (i != this->windowSize)
		{
			printf("�������ڵ�%d��Ϊ���ҿ���\n", i);
			i++;
		}
		cout << "[[[[[[[[[�������ڴ�ӡ����]]]]]]]]]]" << endl;
		return;
	}

	bool SRRdtSender::inPacketWindow(int seqnum) {
		bool result = false;
		int i = 0;
		for (; i < this->packetWindow.size() && this->packetWindow[i].realPacket.seqnum != seqnum; i++);
		if (i<this->packetWindow.size()&&this->packetWindow[i].realPacket.seqnum==seqnum)		//����ҵ���ƥ��ķ��ͱ������к�
		/*��Ҫȷ��i���ܳ���size���Խ��*/
			if (this->packetWindow[i].ackStatus != true) {	//��Ҫȷ����ǰû���յ���ͬ����ȷ�ϣ������ʱ���ظ��رյȴ���ĳ���
				this->packetWindow[i].ackStatus = true;
				result = true;
				/*��Ҫ�����޸�result*/
			}
		return result;
	}

	bool SRRdtSender::getWaitingState() {

		if (this->packetWindow.size() == this->windowSize)
			/*���ʹ�ü򵥵Ĵ�С��ϵ�Ƚϣ�����ģ�������ų�ͻ*/
			return true;
		else
			return false;
		/*�ϲ�Ӧ�ò�����ȿ�waitingstate�Ƿ�Ϊtrue�����SR������������ˣ��൱�ڴ��ڵȴ�״̬����˷���true��Ӧ�ò��յ���ȴ���һ�η��ͣ���֮�򷵻�false
		���Ƹ�Ϊ������Ӧ�����������queue���в���ʱʵʱ�ж��Ƿ񴰿���������������˾ͽ����е�waitingState�����������ã���������ֻ��Ҫ����waitingState�Ϳ���
		�������������������ʱ�临�Ӷȣ��������Ϊ���ı�����*/
	}

	int SRRdtSender::windowSize = 4;
	//��SRRdtSender�Ķ���֮�ⶨ��static�������ͣ��淶���ڵĴ�С�������еı�����queue���й�������windowSize���бȽ�
	//��ʵ�÷Ǿ�̬��Ա����Ҳ���ԣ�����ֻ�Ƕ�һ�ֳ���

	bool SRRdtSender::send(const Message& message) {
		if (this->packetWindow.size() == this->windowSize) { //���ͷ��Ĵ����������ܾ�����
		/*���ʹ�ü򵥵Ĵ�С��ϵ�Ƚϣ�����ģ�������ų�ͻ*/
			return false;
		}
		else {
			const SRPacket& insertPacket = SRPacket::SRPacket();
			this->packetWindow.push_back(insertPacket);
			this->packetWindow.back().realPacket.acknum = -1; //���Ը��ֶ�
			this->packetWindow.back().realPacket.seqnum = this->nextSequenceNumber;
			this->packetWindow.back().realPacket.checksum = 0;
			memcpy(this->packetWindow.back().realPacket.payload, message.data, sizeof(message.data));
			this->packetWindow.back().realPacket.checksum = pUtils->calculateCheckSum(this->packetWindow.back().realPacket);
			pUtils->printPacket("���ͷ����ͱ���", this->packetWindow.back().realPacket);
			pns->sendToNetworkLayer(RECEIVER, this->packetWindow.back().realPacket);								//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�
			this->printSlideWindow();
			/*����������������ʱ����*/
			pns->startTimer(SENDER, Configuration::TIME_OUT, this->packetWindow.back().realPacket.seqnum);	//ֻ�Ըշ��͵ı��Ķ�Ӧ��������ó�ʱ���������ͷ���ʱ��

			this->nextSequenceNumber = (++this->nextSequenceNumber) % 8; //���������0-7֮���л�
			/*++ǧ���ŵ����棡*/
			/*���������ʹ��ڴ�С��ͬ������ܻ���Ϊȷ�ϱ����Դ��ڴ�СΪ��λ��ʧ�����������ݱ����Դ��ڴ�СΪ��λ�ظ�����*/
			return true;
		}
	}

	void SRRdtSender::receive(const Packet& ackPkt) {
		//���У����Ƿ���ȷ
		int checkSum = pUtils->calculateCheckSum(ackPkt);

		//���У�����ȷ�͵���base�ͼ�ʱ��
		/*���ң�ֻ����ȷ�����=���ͷ��ѷ��Ͳ��ȴ�ȷ�ϵ����ݰ���Ųų����У�*/
		if (checkSum == ackPkt.checksum) {
			{
				pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
				if (inPacketWindow(ackPkt.acknum))
					//���ȷ�ϱ��ĵ����к��ڵȴ������У���ֹͣ��ʱ������������ƶ���������
				{
					pns->stopTimer(SENDER, ackPkt.acknum);		//�ر�ACK���Ķ�Ӧ�Ķ�ʱ��
				/*ע��ACK���ĵ����к����԰���acknum��Ա*/
					while (this->packetWindow.size() != 0 && this->packetWindow.front().ackStatus == true)
						//�ƶ���������ֱ����һ��δ�յ�ȷ�ϵĳ�Ա�򴰿���δ�յ�ȷ�ϳ�ԱΪ��
					{
						this->packetWindow.erase(this->packetWindow.begin());
						this->printSlideWindow();
						this->base = (++this->base) % 8;
					}
				}
				//����ÿ���ѷ��ͱ��Ķ��е����ļ�ʱ������˲�������GBNһ���ж��Ƿ�������ʱ��
				else cout << "���ͷ��յ���ȷ�����ϵ�ȷ��/�Ѿ��õ���ȷ��" << endl;
			}
		}
		else
			pUtils->printPacket("���ͷ��յ��İ�У��ʹ���", ackPkt);//SR���������У���ϳ����˴���͵��������ʾ��ʲôҲ����
	}

	void SRRdtSender::timeoutHandler(int seqNum) {
		//��SR��,ÿ�����͵�û���յ�ȷ�ϵı�����Ψһ�Ķ�ʱ��,��Ҫ����seqNum
		cout << "���ͷ���Ӧ���Ķ�ʱ��ʱ�䵽�����·�����ݳ�ʱ�ı���" << endl;
		int i;
		for (i = 0; this->packetWindow[i].realPacket.seqnum != seqNum; i++);
		pUtils->printPacket("���ģ�", this->packetWindow[i].realPacket);
		/*����Ҫ��ȷѡȡ��*/

		pns->stopTimer(SENDER, seqNum);										//���ȹرն�ʱ��
		pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);			//�����������ͷ���ʱ��

		pns->sendToNetworkLayer(RECEIVER, this->packetWindow[i].realPacket);			//���·��ʹ����г�ʱ��������ݰ�

	}
#endif
