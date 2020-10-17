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
#include "SRRdtReceiver.h"
#include "SRRdtSender.h"

int main(int argc, char* argv[])
{
	int batMode = 1;
	//为1表示当前使用了批处理程序检查

	RdtSender* ps = NULL;
	RdtReceiver* pr = NULL;
	FILE* newOut = NULL;

#ifndef NONE_GBN_RDT_SENDER_MODE
	ps = new GBNRdtSender();
	pr = new GBNRdtReceiver();
	cout << "GBN Online" << endl;
	//指示当前的模式
	if (batMode == 0)
		freopen_s(&newOut, "result.txt", "w", stdout);
	//如果没有使用批处理检查，则将输出重定向到文件
#endif // !NONE_GBN_RDT_SENDER_MODE

#ifndef NONE_SR_RDT_SENDER_MODE
	ps = new SRRdtSender();
	pr = new SRRdtReceiver();
	cout << "SR Online" << endl;
	if(batMode==0)
		freopen_s(&newOut, "result.txt", "w", stdout);
#endif // !NONE_SR_RDT_SENDER_MODE

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
	if(newOut!=NULL)
		fclose(newOut);
	/*必须要进行判断，因为你不能嗯关一个NULL的文件指针指向的文件*/
	return 0;
}

