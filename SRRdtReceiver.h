#ifndef SR_RDT_RECEIVER_H			//保证头文件的内容相同项只被包含一次，可与上行同时使用
#define SR_RDT_RECEIVER_H			
#include "RdtReceiver.h"
class SRRdtReceiver : public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;	// 期待收到的下一个报文序号
	Packet lastAckPkt;				//上次发送的确认报文

public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();

public:

	void receive(const Packet& packet);	//接收报文，将被NetworkService调用
};

#endif