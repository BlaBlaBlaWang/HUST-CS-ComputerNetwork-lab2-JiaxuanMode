// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣
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
	//Ϊ1��ʾ��ǰʹ���������������

	RdtSender* ps = NULL;
	RdtReceiver* pr = NULL;
	FILE* newOut = NULL;

#ifndef NONE_GBN_RDT_SENDER_MODE
	ps = new GBNRdtSender();
	pr = new GBNRdtReceiver();
	cout << "GBN Online" << endl;
	//ָʾ��ǰ��ģʽ
	if (batMode == 0)
		freopen_s(&newOut, "result.txt", "w", stdout);
	//���û��ʹ���������飬������ض����ļ�
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
	if(newOut!=NULL)
		fclose(newOut);
	/*����Ҫ�����жϣ���Ϊ�㲻���Ź�һ��NULL���ļ�ָ��ָ����ļ�*/
	return 0;
}

