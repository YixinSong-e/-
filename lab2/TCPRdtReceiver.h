#pragma once
#ifndef TCPRDTRECEIVER_H
#define TCPRDTRECEIVER_H
#include "RdtReceiver.h"

class TCPRdtReceiver :public RdtReceiver {
private:
	int expectSequenceNumberRcvd;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	TCPRdtReceiver();
	virtual ~TCPRdtReceiver();

public:

	void receive(const Packet &packet);	//���ձ��ģ�����NetworkService����
};
#endif // !TCPRDTRECEIVER_H
