#include "stdafx.h"
#ifndef NONE_TCP_RDT_SENDER_MODE	//������Ԥ�������֮�󣬼������У��������Ϊ�������Ѿ��������ͷ�ļ���������
#include "Global.h"
#include "TCPRdtReceiver.h"

	TCPRdtReceiver::TCPRdtReceiver() :expectSequenceNumberRcvd(0)
	{
		lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
		lastAckPkt.checksum = 0;
		lastAckPkt.seqnum = -1;	//���Ը��ֶ�
		for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
			lastAckPkt.payload[i] = '.';
		}
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
	
		//��ʼ״̬�ڴ����յ��ı���Ϊ1��
		this->expectSequenceNumberRcvd = 0;
	}


	TCPRdtReceiver::~TCPRdtReceiver()
	{
	}

	void TCPRdtReceiver::receive(const Packet& packet) {
		//���У����Ƿ���ȷ
		int checkSum = pUtils->calculateCheckSum(packet);

		//���У�����ȷ��ͬʱ�յ����ĵ���ŵ��ڽ��շ��ڴ��յ��ı������һ��
		if (checkSum == packet.checksum && this->expectSequenceNumberRcvd == packet.seqnum) {
			pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);

			//ȡ��Message�����ϵݽ���Ӧ�ò�
			Message msg;
			memcpy(msg.data, packet.payload, sizeof(packet.payload));
			pns->delivertoAppLayer(RECEIVER, msg);
			
			lastAckPkt.acknum = packet.seqnum; //ȷ����ŵ����յ��ı������
			lastAckPkt.seqnum = packet.seqnum;
			memcpy(lastAckPkt.payload, packet.payload, sizeof(packet.payload));
			lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
			pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�

			this->expectSequenceNumberRcvd = (++this->expectSequenceNumberRcvd)% 8; 
			/*�ͷ��ͷ����ڴ�С��Ӧ�����������0-3֮���л�*/
		}
		else {
			if (checkSum != packet.checksum) {
				pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
			}
			else {
				pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,������Ų���", packet);
			}
			pUtils->printPacket("���շ����·����ϴε�ȷ�ϱ���", lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢���ϴε�ȷ�ϱ���

		}
	}
#endif