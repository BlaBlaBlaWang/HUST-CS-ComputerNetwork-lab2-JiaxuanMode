//#pragma once						//保证头文件的实体只被包含一次，如果没有设置好预编译头，不加上VS会有报错
#ifndef GBN_RDT_RECEIVER_H			//保证头文件的内容相同项只被包含一次，可与上行同时使用
#define GBN_RDT_RECEIVER_H			
#include "RdtReceiver.h"
class GBNRdtReceiver : public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;	// 期待收到的下一个报文序号
	Packet lastAckPkt;				//上次发送的确认报文

public:
	GBNRdtReceiver();
	virtual ~GBNRdtReceiver();

public:

	void receive(const Packet& packet);	//接收报文，将被NetworkService调用
};

#endif