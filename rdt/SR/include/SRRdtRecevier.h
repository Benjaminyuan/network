#ifndef GBN_RDT_RECEIVER_H
#define GBN_RDT_RECEIVER_H
#include<unordered_map> 
#include "RdtReceiver.h"
using namespace std;
class SRRdtReceiver :public RdtReceiver
{
private:
	int recv_base;	// 期待收到的下一个报文序号
	Packet sndPkt;
	unordered_map<int,Packet> recv_map;
public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();

public:
	void receive(const Packet &packet);	//接收报文，将被NetworkService调用
};

#endif

