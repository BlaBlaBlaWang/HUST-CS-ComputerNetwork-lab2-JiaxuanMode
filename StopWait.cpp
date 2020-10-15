// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣
//
//#define GBN_RDT_RECEIVER_H 1
//#define GBN_RDT_SENDER_H 1
//#define NONE_GBN_RDT_SENDER_MODE 1
#define STOP_WAIT_RDT_RECEIVER_H 1
#define STOP_WAIT_RDT_SENDER_H 1
#define NONE_STOP_WAIT_RDT_SENDER_MODE 1
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

	//ps = new GBNRdtSender();
	//pr = new GBNRdtReceiver();

	ps = new StopWaitRdtSender();
	pr = new StopWaitRdtReceiver();


//	pns->setRunMode(0);  //VERBOSģʽ
	pns->setRunMode(1);  //����ģʽ
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("input.txt");
	pns->setOutputFile("output.txt");

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete
	
	return 0;
}

