#ifndef SR_RDT_RECEIVER_H			//��֤ͷ�ļ���������ͬ��ֻ������һ�Σ���������ͬʱʹ��
#define SR_RDT_RECEIVER_H			
#include "RdtReceiver.h"
class SRRdtReceiver : public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();

public:

	void receive(const Packet& packet);	//���ձ��ģ�����NetworkService����
};

#endif