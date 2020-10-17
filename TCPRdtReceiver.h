#ifndef TCP_RDT_RECEIVER_H			//��֤ͷ�ļ���������ͬ��ֻ������һ�Σ���������ͬʱʹ��
#define TCP_RDT_RECEIVER_H			
#include "RdtReceiver.h"
class TCPRdtReceiver : public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	TCPRdtReceiver();
	virtual ~TCPRdtReceiver();

public:

	void receive(const Packet& packet);	//���ձ��ģ�����NetworkService����
};

#endif