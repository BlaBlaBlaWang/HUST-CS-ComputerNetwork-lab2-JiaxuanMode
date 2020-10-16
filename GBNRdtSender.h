#ifndef GBN_RDT_SENDER_H
#define GBN_RDT_SENDER_H
#include "RdtSender.h"
using std::queue;

class GBNRdtSender :public RdtSender
{
private:
	int nextSequenceNumber;			// ��һ��������� 
	bool waitingState;				// �Ƿ��ڵȴ�Ack��״̬
	static int windowSize;
	queue <Packet> packetWindow;	//�ѷ��Ͳ��ȴ�Ack�����ݰ�queue�����ݴ��ڴ�С��һ��ά��4��ʵ����queue����
	int base;						//��¼���ʹ��ڵĵ�ǰ���

public:

	bool getWaitingState();
	bool send(const Message& message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet& ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����

public:
	GBNRdtSender();
	virtual ~GBNRdtSender();
};

#endif