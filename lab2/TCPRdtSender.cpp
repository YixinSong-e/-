//#include "stdafx.h"
#include <cstring>
#include "Global.h"
#include <assert.h>
#include "TCPRdtSender.h"

const bool test = true;
bool TCPRdtSender::getWaitingState()
{
	return this->waitingState;
}
TCPRdtSender::TCPRdtSender() :nextseqnum(1), waitingState(false), base(1) {}
TCPRdtSender::~TCPRdtSender() {}
bool TCPRdtSender::send(const Message &message)
{
	if (packets.size() >= GBN_WINDOW_SIZE)
	{
		return false; //������Ҫ�ȴ�
	}
	Packet * p = new Packet();
	p->acknum = -1;
	p->checksum = 0;
	p->seqnum = this->nextseqnum;
	memcpy(p->payload, message.data, sizeof(message.data));
	p->checksum = pUtils->calculateCheckSum(*p);
	packets.push_back(p);
	pUtils->printPacket("���ͷ����ͱ���", *p); //����
	if (this->base == this->nextseqnum)
	{
		pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	}
	this->nextseqnum++;
	nextseqnum %= MAX_SEQ;
	if (packets.size() >= GBN_WINDOW_SIZE) waitingState = true;
	pns->sendToNetworkLayer(RECEIVER, *p);
	p = nullptr;
	return true;

}
void TCPRdtSender::receive(const Packet &ackPkt)
{
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum)
	{
		if (((nextseqnum > base) && (ackPkt.acknum >= base) && (ackPkt.acknum < nextseqnum)) || ((nextseqnum < base) && (ackPkt.acknum >= 0) && ((ackPkt.acknum < nextseqnum) || (ackPkt.acknum >= base)) ))
		{
			pUtils->printPacket("���ͷ��յ�ACK�ظ�", ackPkt);
			waitingState = false;
			int tmp = base;
			printf("���ͷ����ڱ仯ǰ\n");
			printWindow();
			//printf("base is %d\n", base);
			base = ackPkt.acknum + 1;
			base %= MAX_SEQ;
			int index = 0;
			if (ackPkt.acknum >= tmp)  index = ackPkt.acknum - tmp;
			else  index = MAX_SEQ - tmp + ackPkt.acknum;
			//printf("GetAck is %d\n", ackPkt.acknum);
			//printf("index is :%d\n", index);
			//for (int i = 0; i < packets.size(); i++) pUtils->printPacket("�����������ݱ�:", *packets[i]);
			for (int i = 0; i <= index; i++) delete packets.at(i);

			packets.erase(packets.begin(), packets.begin() + index + 1);
			printf("���ͷ����ڷ����仯��\n");
			printWindow();

			//printf("���д�СΪ%d\n", packets.size());
			//for (int i = 0; i < packets.size(); i++) pUtils->printPacket("�����������ݱ�:", *packets[i]);

			//��ʱ��ֹͣ������
			if (base == nextseqnum) pns->stopTimer(SENDER, 0);
			else
			{
				pns->stopTimer(SENDER, 0);
				pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
			}
			ackCount[ackPkt.acknum] = 1;

		}
		else
		{
			ackCount[ackPkt.acknum]++;
			if ((ackCount[ackPkt.acknum] == 4 || ackPkt.acknum == -1) && packets.size() != 0)
			{
				Packet * packet = packets.at(0);
				pUtils->printPacket("�յ�3���ظ�ack��ִ�п����ش�", *packet);
				std::cout << "�յ��ظ�ack = " << ackPkt.acknum << "ִ�п����ش�" << std::endl;
				
				pns->sendToNetworkLayer(RECEIVER, *packet);
				ackCount[ackPkt.acknum] = 0;
			}
		}
	}
}
//��ʱ����
void TCPRdtSender::timeoutHandler(int seqNum)
{
	pns->stopTimer(SENDER, seqNum);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
	for (int i = 0; i < packets.size(); i++)
	{
		Packet * p = packets[i];
		pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط��ϴη��͵ı���", *p);
		pns->sendToNetworkLayer(RECEIVER, *p);
	}
}
void TCPRdtSender::printWindow()
{
	if (base < nextseqnum)
	{
		std::cout << "(";
		for (int i = base; i < nextseqnum; i++) std::cout << " " << i;
		std::cout << " )" << std::endl;
	}
	else if (base > nextseqnum)
	{
		std::cout << "(";
		for (int i = base; i < MAX_SEQ; i++) std::cout << " " << i;
		for (int i = 0; i < nextseqnum; i++) std::cout << " " << i;
		std::cout << " )" << std::endl;
	}
	else std::cout << "( )" << std::endl;
}