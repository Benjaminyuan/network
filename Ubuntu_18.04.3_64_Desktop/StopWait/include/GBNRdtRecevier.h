#ifndef GBN_RDT_RECEIVER_H
#define GBN_RDT_RECEIVER_H
#include "RdtReceiver.h"
class GBNRdtReceiver :public RdtReceiver
{
private:
	int expectSeqNum;	// 期待收到的下一个报文序号
	Packet sndPkt;
public:
	GBNRdtReceiver();
	virtual ~GBNRdtReceiver();

public:
	void receive(const Packet &packet);	//接收报文，将被NetworkService调用
};

#endif

