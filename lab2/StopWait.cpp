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
#include "TCPRdtReceiver.h"
#include "TCPRdtSender.h"

int main(int argc, char* argv[])
{
	RdtSender *ps = new TCPRdtSender();
	RdtReceiver *pr = new TCPRdtReceiver();
	freopen("data.txt", "w", stdout);
//	RdtSender *ps = new SRRdtSender();
//	RdtReceiver *pr = new SRRdtReceiver();
//	RdtSender *ps = new GBNRdtSender();
//	RdtReceiver * pr = new GBNRdtReceiver();
	//RdtSender *ps = new StopWaitRdtSender();
	//RdtReceiver * pr = new StopWaitRdtReceiver();
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

