#pragma once
#ifndef  GBNRDTRECEIVER_H
#define GBNRDTRECEIVER_H
#include "RdtReceiver.h"

class GBNRdtReceiver :public RdtReceiver {
private:
	int expectSequenceNumberRcvd;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	GBNRdtReceiver();
	virtual ~GBNRdtReceiver();

public:

	void receive(const Packet &packet);	//���ձ��ģ�����NetworkService����
};
#endif // ! GBNRDTRECEIVER_H
