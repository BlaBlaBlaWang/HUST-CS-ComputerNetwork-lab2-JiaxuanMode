#ifndef SR_RDT_SENDER_H
#define SR_RDT_SENDER_H
#include "RdtSender.h"
using std::queue;

class SRPacket
{
public:
	struct Packet realPacket;
	bool ackStatus;	//�ڷ��ͷ��Ĵ�����������¼�ñ����Ƿ��Ѿ������գ��ڽ��շ��Ĵ�����������¼�ñ����Ƿ��Ѿ��յ�

	SRPacket() {
		ackStatus = false;
	}
	~SRPacket() {

	}
	/*���û�ж����������������ӱ���ʵ���ϸ����յĺ�������У�ʣ�µĽ���������*/
};


class SRRdtSender :public RdtSender
{
private:
	int nextSequenceNumber;			// ��һ��������� 
	bool waitingState;				// �Ƿ��ڵȴ�Ack��״̬
	static int windowSize;
	vector <SRPacket> packetWindow;	//�ѷ��Ͳ��ȴ�Ack�����ݰ�vector�����ݴ��ڴ�С��һ��ά��4��ʵ����vector����
	/*�м�ǣ������������޸�ackbun�Ȳ�������˲���ʹ������������ʵ�queue*/
	int base;						//��¼���ʹ��ڵĵ�ǰ���

public:

	bool getWaitingState();
	bool send(const Message& message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet& ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����
	void printSlideWindow();
	bool inPacketWindow(int seqnum);		//�жϸ�����seq��Ŷ�Ӧ�ı����ڲ��ڷ��Ͷ�����

public:
	SRRdtSender();
	virtual ~SRRdtSender();
};

#endif