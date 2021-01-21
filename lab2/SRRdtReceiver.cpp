#include "SRRdtReceiver.h"
SRRdtReceiver::SRRdtReceiver() :base(1), expectSequenceNumberRcvd(1)
{
	lastAckPkt.acknum = -1;
	lastAckPkt.seqnum = -1;
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) 
	{
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
	//初始化接受方窗口
	for (int i = 0; i < SR_WINDOW_SIZE; i++)
	{
		Packet *p = new Packet();
		while (p)
		{
			buffers.push_back(p);
			flags.push_back(0);
			break;
		}
		p = nullptr;	
	}
}
SRRdtReceiver::~SRRdtReceiver(){}

void SRRdtReceiver::receive(const Packet &packet)
{
	int checkSum = pUtils->calculateCheckSum(packet);
	if (checkSum == packet.checksum)
	{
		bool case1 = false;
		bool case2 = false;
		int index = 0;
		//1
		if (base + SR_WINDOW_SIZE <= MAX_SEQ)
		{
			if (packet.seqnum >= base && packet.seqnum < base + SR_WINDOW_SIZE)
			{
				case1 = true;
				index = packet.seqnum - base;
			}
		}
		else
		{
			if ((packet.seqnum + GBN_WINDOW_SIZE) % MAX_SEQ >= (base + GBN_WINDOW_SIZE) % MAX_SEQ &&
				(packet.seqnum + GBN_WINDOW_SIZE) % MAX_SEQ <= (base + 2 * GBN_WINDOW_SIZE - 1) % MAX_SEQ)
			{
				case1 = true;
				index = (packet.seqnum + GBN_WINDOW_SIZE) % MAX_SEQ - (base + GBN_WINDOW_SIZE) % MAX_SEQ;
			}
		}
		//2
		if (base >= GBN_WINDOW_SIZE) {
			// 此时base距离序号0至少还有N个空间
			if (packet.seqnum >= base - GBN_WINDOW_SIZE && packet.seqnum <= base - 1) {
				case2 = true;
			}
		}
		else {
			// 如果base距离0不足N个空间，那么两者一起加上N来跨越边界
			if ((packet.seqnum + GBN_WINDOW_SIZE) % MAX_SEQ >= base &&
				(packet.seqnum + GBN_WINDOW_SIZE) % MAX_SEQ <= base + GBN_WINDOW_SIZE - 1) {
				case2 = true;
			}
		}

		//处理
		if (case1)
		{
			lastAckPkt.acknum = packet.seqnum;
			lastAckPkt.checksum = 0;
			lastAckPkt.checksum = pUtils->calculateCheckSum(packet);
			memcpy(lastAckPkt.payload, packet.payload, sizeof(packet.payload));
			pUtils->printPacket("接收方收到报文", packet);
			pUtils->printPacket("接收方发送确认报文", lastAckPkt);
			pns->sendToNetworkLayer(SENDER, lastAckPkt);
			if (flags[index] == 0)
			{
				flags[index] = 1;
				Packet *tmp = buffers[index];
				copy_packet(&packet, tmp);
				tmp = nullptr;
			}
		}
		if (case2)
		{
			lastAckPkt.acknum = packet.seqnum;
			lastAckPkt.checksum = 0;
			lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
			pUtils->printPacket("接收方发送确认报文", lastAckPkt); 
			pns->sendToNetworkLayer(SENDER, lastAckPkt);    //调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
			printf("虽然接受方已经确认过该报文，按照协议再次发送!\n");
		}
		if (case1)
		{
			int k = 0;
			if (flags.at(0) != 1)
			{
				printf("缓存对应包,缓存后接收方窗口如下\n");
				printWindow();
			}
			else
			{
				k = 1;
				printf("现向上集体发包\n");
				printWindow();
				
			}
			while (flags.at(0) == 1)
			{
				//printf("接收方上交一个报文\n");
				
				Message msg;
				memcpy(msg.data, buffers.at(0)->payload, sizeof(buffers.at(0)->payload));
				pns->delivertoAppLayer(RECEIVER, msg);
				buffers.erase(buffers.begin());
				flags.erase(flags.begin());
				Packet *p = new Packet();
				buffers.push_back(p);
				flags.push_back(0);
				p = nullptr;
				base++;
				base %= MAX_SEQ;
			}
			if (k == 1)
			{
				printf("发包后缓存如下\n");
				printWindow();
			}
		}
	}
}
void SRRdtReceiver::copy_packet(const Packet *source, Packet *dest) {
	dest->checksum = source->checksum;
	dest->acknum = source->acknum;
	dest->seqnum = source->acknum;
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		dest->payload[i] = source->payload[i];
	}
	//assert(pUtils->calculateCheckSum(*source) == source->checksum);
}
void SRRdtReceiver::printWindow() 
{
		if (flags.size() == 0) 
		{
			std::cout << "( )" << std::endl;
			return;
		}
		std::cout << "( ";
		for (int i = 0; i < flags.size(); i++) 
		{
			if (flags.at(i) == 1) 
			{
				std::cout << "'" << (i + base) % MAX_SEQ << "' ";
			}
			else 
			{
				std::cout << (i + base) % MAX_SEQ << " ";
			}
		}
		std::cout << ")" << std::endl;
}