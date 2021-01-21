#pragma once
#ifndef GBNRDTSENDER_H
#define GBNRDTSENDER_H
#include"RdtSender.h"
#include<vector>
class GBNRdtSender :public RdtSender {
private:
	bool waitingState;				// �Ƿ��ڵȴ�Ack��״̬
	int base;
	int nextseqnum;
	std::vector<Packet *> packets;
	void printWindow();
public:
	bool getWaitingState();
	bool send(const Message &message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet &ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����
public:
	GBNRdtSender();
	virtual ~GBNRdtSender();


};
#endif // !GBNRDTSENDER_H
