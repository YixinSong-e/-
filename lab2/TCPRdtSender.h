#pragma once
#pragma once
#ifndef TCPRDTSENDER_H
#define TCPRDTSENDER_H
#include"RdtSender.h"
#include<vector>
#include<map>
class TCPRdtSender :public RdtSender {
private:
	bool waitingState;				// �Ƿ��ڵȴ�Ack��״̬
	int base;
	int nextseqnum;
	std::vector<Packet *> packets;
	std::map<int, int>ackCount;
	void printWindow();
public:
	bool getWaitingState();
	bool send(const Message &message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet &ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����
public:
	TCPRdtSender();
	virtual ~TCPRdtSender();


};
#endif // !TCPRDTSENDER_H
