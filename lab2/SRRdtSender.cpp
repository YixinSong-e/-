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
	pUtils->printPacket("发送方发送报文", *p); //测试
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
			printf("发送包%d标记为收到\n", ackPkt.acknum);
			flags[index] = 1;
			if (ackPkt.acknum == base)
			{
				printf("发送方窗口变化前\n");
				printWindow();
				while (packets.size() > 0 && flags[0] == true)
				{
					packets.erase(packets.begin());
					flags.erase(flags.begin());
					base++;
				}
				base %= MAX_SEQ;
				printf("发送方窗口变化后\n");
				printWindow();
			}
			//base %= MAX_SEQ;
			//printf("窗口变化后\n");
			//printWindow();
			pUtils->printPacket("发送方接收到回应ACK", ackPkt);
		}
		else
			std::cout << "接收到ACK不在发送方窗口内,ack = " << ackPkt.acknum << "base = " << base << std::endl;
	}
	waitingState = packets.size() >= GBN_WINDOW_SIZE;
}
bool SRRdtSender::getWaitingState() 
{
	return waitingState;
}
void SRRdtSender::timeoutHandler(int seqNum)
{
	printf("序号%d超时,重新发送\n", seqNum);
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