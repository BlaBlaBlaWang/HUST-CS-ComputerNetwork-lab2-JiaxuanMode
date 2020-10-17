#ifndef SR_RDT_RECEIVER_H			//��֤ͷ�ļ���������ͬ��ֻ������һ�Σ���������ͬʱʹ��
#define SR_RDT_RECEIVER_H			
#include "RdtReceiver.h"

class SRRdtReceiver : public RdtReceiver
{
private:
	static int windowSize;
	vector <Packet> packetWindow;	//���ղ���������ݰ�vector�����ݴ��ڴ�С��һ��ά��4��ʵ����queue����
	int base;						//��¼���ʹ��ڵĵ�ǰ���

public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();
	bool seqNumNew(int seqnum);	//�����жϽ��շ��յ��ı����е�������Լ��Ѿ�����������Ż�����Ҫ���������ţ��Ӷ�ѡ��ͬ�Ĵ���ʽ
	//����false����Ϊ����������ţ��򵥴�����ack���ɣ�����true��Ҫ����Ȳ���
	void savePktInOrder(Packet packet);	//���򻺴������vector��
	int modOrder(int seqnum);	//��base����ģ�����õ�������order
	void printSlideWindow(void);	//��ӡ��������

public:

	void receive(const Packet& packet);	//���ձ��ģ�����NetworkService����
};

#endif