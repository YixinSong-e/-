#pragma once
#ifndef SRRDTRECEIVER_H
#define SRRDTRECEIVER_H
#include "RdtReceiver.h"
#include "Global.h"
class SRRdtReceiver :public RdtReceiver {
private:
	int expectSequenceNumberRcvd;
	std::vector<Packet *> buffers;
	std::vector<int> flags;
	Packet lastAckPkt;
	int base;
	void printWindow();
	void copy_packet(const Packet * source, Packet * dest);
public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();
public:
	void receive(const Packet &packet);
};
#endif // !SRRDTRECEIVER_H
