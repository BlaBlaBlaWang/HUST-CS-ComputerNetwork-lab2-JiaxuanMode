#ifndef SR_RDT_SENDER_H
#define SR_RDT_SENDER_H
#include "RdtSender.h"
using std::queue;

class SRPacket
{
public:
	struct Packet realPacket;
	bool ackStatus;	//在发送方的窗口中用来记录该报文是否已经被接收，在接收方的窗口中用来记录该报文是否已经收到

	SRPacket() {
		ackStatus = false;
	}
	~SRPacket() {

	}
	/*起初没有定义析构方法，链接报错，实际上给个空的函数体就行，剩下的交给编译器*/
};


class SRRdtSender :public RdtSender
{
private:
	int nextSequenceNumber;			// 下一个发送序号 
	bool waitingState;				// 是否处于等待Ack的状态
	static int windowSize;
	vector <SRPacket> packetWindow;	//已发送并等待Ack的数据包vector，根据窗口大小，一共维护4个实例的vector数组
	/*中间牵扯到随机访问修改ackbun等操作，因此不能使用难以随机访问的queue*/
	int base;						//记录发送窗口的当前序号

public:

	bool getWaitingState();
	bool send(const Message& message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(const Packet& ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);					//Timeout handler，将被NetworkServiceSimulator调用
	void printSlideWindow();
	bool inPacketWindow(int seqnum);		//判断给定的seq序号对应的报文在不在发送队列中

public:
	SRRdtSender();
	virtual ~SRRdtSender();
};

#endif