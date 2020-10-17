#ifndef SR_RDT_RECEIVER_H			//保证头文件的内容相同项只被包含一次，可与上行同时使用
#define SR_RDT_RECEIVER_H			
#include "RdtReceiver.h"

class SRRdtReceiver : public RdtReceiver
{
private:
	static int windowSize;
	vector <Packet> packetWindow;	//接收并缓存的数据包vector，根据窗口大小，一共维护4个实例的queue队列
	int base;						//记录发送窗口的当前序号

public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();
	bool seqNumNew(int seqnum);	//用来判断接收方收到的报文中的序号是自己已经滑过的老序号还是需要缓存的新序号，从而选择不同的处理方式
	//返回false表明为滑过的老序号，则单纯返回ack即可，返回true则要缓存等操作
	void savePktInOrder(Packet packet);	//按序缓存包到包vector中
	int modOrder(int seqnum);	//和base进行模运算差得到真正的order
	void printSlideWindow(void);	//打印滑动窗口

public:

	void receive(const Packet& packet);	//接收报文，将被NetworkService调用
};

#endif