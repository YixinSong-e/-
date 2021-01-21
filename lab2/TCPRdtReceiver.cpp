#include "TCPRdtReceiver.h"
#include "Global.h"
#include <cstring>
//#include "stdafx.h"

TCPRdtReceiver::TCPRdtReceiver() : expectSequenceNumberRcvd(1)
{
	lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;    //���Ը��ֶ�
	for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
}
TCPRdtReceiver::~TCPRdtReceiver() {}
void TCPRdtReceiver::receive(const Packet &packet)
{
	int checkSum = pUtils->calculateCheckSum(packet);
	if (checkSum == packet.checksum && this->expectSequenceNumberRcvd == packet.seqnum)
	{
		pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);
		Message msg;
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);
		lastAckPkt.seqnum = packet.seqnum;
		lastAckPkt.acknum = packet.seqnum;
		lastAckPkt.checksum = packet.checksum;
		pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);
		this->expectSequenceNumberRcvd += 1;
		this->expectSequenceNumberRcvd %= MAX_SEQ;
	}
	else
	{
		if (packet.seqnum != this->expectSequenceNumberRcvd)
		{
			pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,������Ų���", packet);
		}
		else
		{
			pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
		}
		pUtils->printPacket("���շ����·����ϴε�ȷ�ϱ���", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, this->lastAckPkt);
	}
}

