// StopWait.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"
#include "GBNRdtReceiver.h"
#include "GBNRdtSender.h"

int main(int argc, char* argv[])
{
	RdtSender* ps = NULL;
	RdtReceiver* pr = NULL;

#ifndef NONE_GBN_RDT_SENDER_MODE
	ps = new GBNRdtSender();
	pr = new GBNRdtReceiver();
	cout << "GBN Online" << endl;
#endif // !NONE_GBN_RDT_SENDER_MODE

#ifndef NONE_STOP_WAIT_RDT_SENDER_MODE
	ps = new StopWaitRdtSender();
	pr = new StopWaitRdtReceiver();
	cout << "Stop Wait Online" << endl;
#endif // !NONE_STOP_WAIT_RDT_SENDER_MODE

//	pns->setRunMode(0);  //VERBOS模式
	pns->setRunMode(1);  //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("input.txt");
	pns->setOutputFile("output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
	delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
	
	return 0;
}

