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
	//��ʼ�����ܷ�����
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
			// ��ʱbase�������0���ٻ���N���ռ�
			if (packet.seqnum >= base - GBN_WINDOW_SIZE && packet.seqnum <= base - 1) {
				case2 = true;
			}
		}
		else {
			// ���base����0����N���ռ䣬��ô����һ�����N����Խ�߽�
			if ((packet.seqnum + GBN_WINDOW_SIZE) % MAX_SEQ >= base &&
				(packet.seqnum + GBN_WINDOW_SIZE) % MAX_SEQ <= base + GBN_WINDOW_SIZE - 1) {
				case2 = true;
			}
		}

		//����
		if (case1)
		{
			lastAckPkt.acknum = packet.seqnum;
			lastAckPkt.checksum = 0;
			lastAckPkt.checksum = pUtils->calculateCheckSum(packet);
			memcpy(lastAckPkt.payload, packet.payload, sizeof(packet.payload));
			pUtils->printPacket("���շ��յ�����", packet);
			pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
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
			pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt); 
			pns->sendToNetworkLayer(SENDER, lastAckPkt);    //����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�
			printf("��Ȼ���ܷ��Ѿ�ȷ�Ϲ��ñ��ģ�����Э���ٴη���!\n");
		}
		if (case1)
		{
			int k = 0;
			if (flags.at(0) != 1)
			{
				printf("�����Ӧ��,�������շ���������\n");
				printWindow();
			}
			else
			{
				k = 1;
				printf("�����ϼ��巢��\n");
				printWindow();
				
			}
			while (flags.at(0) == 1)
			{
				//printf("���շ��Ͻ�һ������\n");
				
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
				printf("�����󻺴�����\n");
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