#include "Global.h"
#include "SRRdtSender.h"
#include <cstring>

 SRRdtSender::SRRdtSender():base(1), nextseqnum(1), waitingState(false) {}
 SRRdtSender::~SRRdtSender(){}

bool SRRdtSender::send(const Message &message)
{
	if (packets.size() >= GBN_WINDOW_SIZE)
	{
		return false;
	}
	Packet *p = new Packet();
	p->seqnum = nextseqnum;
	p->acknum = -1;
	memcpy(p->payload, message.data, sizeof(message.data));
	p->checksum = pUtils->calculateCheckSum(*p);
	packets.push_back(p);
	flags.push_back(0);
	pUtils->printPacket("���ͷ����ͱ���", *p); //����
	pns->startTimer(SENDER, Configuration::TIME_OUT, p->seqnum);
	this->nextseqnum++;
	nextseqnum %= MAX_SEQ;
	if (packets.size() >= GBN_WINDOW_SIZE) waitingState = true;
	pns->sendToNetworkLayer(RECEIVER, *p);
	p = nullptr;
	return true;

}

void SRRdtSender::receive(const Packet &ackPkt)
{
	
	//pUtils->printPacket("1", ackPkt);
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum == ackPkt.checksum)
	{
		if ((nextseqnum > base && ackPkt.acknum < nextseqnum && ackPkt.acknum >= base) || (nextseqnum < base && ackPkt.acknum >= 0 && (ackPkt.acknum >= base || ackPkt.acknum < nextseqnum) ))
		{
			pns->stopTimer(SENDER, ackPkt.acknum);
			int index = 0;
			if (ackPkt.acknum >= base) index = ackPkt.acknum - base;
			else index = MAX_SEQ - base + ackPkt.acknum;
			printf("���Ͱ�%d���Ϊ�յ�\n", ackPkt.acknum);
			flags[index] = 1;
			if (ackPkt.acknum == base)
			{
				printf("���ͷ����ڱ仯ǰ\n");
				printWindow();
				while (packets.size() > 0 && flags[0] == true)
				{
					packets.erase(packets.begin());
					flags.erase(flags.begin());
					base++;
				}
				base %= MAX_SEQ;
				printf("���ͷ����ڱ仯��\n");
				printWindow();
			}
			//base %= MAX_SEQ;
			//printf("���ڱ仯��\n");
			//printWindow();
			pUtils->printPacket("���ͷ����յ���ӦACK", ackPkt);
		}
		else
			std::cout << "���յ�ACK���ڷ��ͷ�������,ack = " << ackPkt.acknum << "base = " << base << std::endl;
	}
	waitingState = packets.size() >= GBN_WINDOW_SIZE;
}
bool SRRdtSender::getWaitingState() 
{
	return waitingState;
}
void SRRdtSender::timeoutHandler(int seqNum)
{
	printf("���%d��ʱ,���·���\n", seqNum);
	int index = 0;
	if (seqNum >= base) index = seqNum - base;
	else index = MAX_SEQ - base + seqNum;
	pns->sendToNetworkLayer(RECEIVER, *packets[index]);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
}
void SRRdtSender::printWindow()
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