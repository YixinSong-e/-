//#include "stdafx.h"
#include <cstring>
#include "Global.h"
#include <assert.h>
#include "GBNRdtSender.h"

const bool test = true;
bool GBNRdtSender :: getWaitingState()
{
	return this->waitingState;
}
GBNRdtSender::GBNRdtSender() :nextseqnum(1), waitingState(false), base(1) {}
GBNRdtSender::~GBNRdtSender() {}
bool GBNRdtSender::send(const Message &message)
{
	if (packets.size() >= GBN_WINDOW_SIZE)
	{
		return false; //现在需要等待
	}
	Packet * p = new Packet();
	p->acknum = -1;
	p->checksum = 0;
	p->seqnum = this->nextseqnum;
	memcpy(p->payload, message.data, sizeof(message.data));
	p->checksum = pUtils->calculateCheckSum(*p);
	packets.push_back(p);
	pUtils->printPacket("发送方发送报文", *p); //测试
	if (this->base == this->nextseqnum)
	{
		pns->startTimer(SENDER, Configuration::TIME_OUT, p->seqnum);
	}
	this->nextseqnum++;
	nextseqnum %= MAX_SEQ;
	if (packets.size() >= GBN_WINDOW_SIZE) waitingState = true;
	pns->sendToNetworkLayer(RECEIVER, *p);
	p = nullptr;
	return true;

}
void GBNRdtSender::receive(const Packet &ackPkt) 
{
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum)
	{
		if (((nextseqnum > base) && (ackPkt.acknum >= base) && (ackPkt.acknum < nextseqnum)) || ((nextseqnum < base) && ackPkt.acknum >= 0 && ((ackPkt.acknum < nextseqnum) || (ackPkt.acknum >= base))))
		{
			pUtils->printPacket("发送方收到ACK回复", ackPkt);
			waitingState = false;
			int tmp = base;
			//printf("base is %d\n", base);
			printf("收到理想ack，窗口变化前\n");
			printWindow();
			base = ackPkt.acknum + 1;
			base %= MAX_SEQ;
			printf("收到理想ack，窗口变化后\n");
			printWindow();
			int index = 0;
			if (ackPkt.acknum >= tmp)  index = ackPkt.acknum - tmp;
			else  index = MAX_SEQ - tmp + ackPkt.acknum;
			//printf("GetAck is %d\n", ackPkt.acknum);
			//printf("index is :%d\n", index);
			//for (int i = 0; i < packets.size(); i++) pUtils->printPacket("队列中有数据报:", *packets[i]);
			for (int i = 0; i <= index; i++) delete packets.at(i);
			
			packets.erase(packets.begin(), packets.begin() + index + 1);

			//printf("队列大小为%d\n", packets.size());
			//for (int i = 0; i < packets.size(); i++) pUtils->printPacket("队列中有数据报:", *packets[i]);

			//计时器停止与重启
			if(base == nextseqnum) pns->stopTimer(SENDER, tmp);
			else
			{
				pns->stopTimer(SENDER, tmp);
				pns->startTimer(SENDER, Configuration::TIME_OUT, base);
			}

		}
	}
}
//超时处理
void GBNRdtSender::timeoutHandler(int seqNum)
{
	pns->stopTimer(SENDER, seqNum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
	for (int i = 0; i < packets.size(); i++)
	{
		Packet * p = packets[i];
		pUtils->printPacket("发送方定时器时间到，重发上次发送的报文", *p);
		pns->sendToNetworkLayer(RECEIVER, *p);
	}
}
void GBNRdtSender::printWindow()
{
	if (base < nextseqnum)
	{
		std::cout << "(" ;
		for (int i = base; i < nextseqnum; i++) std::cout << " " << i;
		std::cout << " )" << std::endl;
	}
	else if (base > nextseqnum)
	{
		std::cout << "(" ;
		for (int i = base; i < MAX_SEQ; i++) std::cout << " " << i;
		for (int i = 0; i < nextseqnum; i++) std::cout << " " << i;
		std::cout << " )" << std::endl;
	}
	else std::cout << "( )" << std::endl;
}