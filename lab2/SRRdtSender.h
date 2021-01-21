#pragma once
#ifndef SRRDTSENDER_H
#define SRRDTSENDER_H
#include "RdtSender.h"
#include <vector>
class SRRdtSender :public RdtSender 
{
	bool waitingState;				// �Ƿ��ڵȴ�Ack��״̬
	int base;
	int nextseqnum;
	std::vector<Packet *> packets;
	std::vector<bool> flags; // 0����δ֪��1����ȷ��
	void printWindow();
public:
	bool getWaitingState();
	bool send(const Message &message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet &ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����
public:
	SRRdtSender() ;
	virtual ~SRRdtSender() ;
};


#endif // !SRRDTSENDER_H
