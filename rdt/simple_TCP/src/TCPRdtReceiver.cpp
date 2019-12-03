#include "Global.h"
#include"TCPRdtRecevier.h"
#include<iostream>
TCPRdtReceiver::TCPRdtReceiver():expectSeqNum(1){
	sndPkt.acknum = 0;
	sndPkt.checksum = 0;
	sndPkt.seqnum = -1;
	for(int i = 0; i < Configuration::PAYLOAD_SIZE;i++){
		sndPkt.payload[i] = '.';
	}
	sndPkt.checksum = pUtils->calculateCheckSum(sndPkt);
}
TCPRdtReceiver::~TCPRdtReceiver(){

}
void TCPRdtReceiver::receive(const Packet &packet){
	int checkSum = pUtils->calculateCheckSum(packet);
	//正确收到 包
	if(checkSum == packet.checksum && expectSeqNum == packet.seqnum){
		pUtils->printPacket("接收方正确收到发送方的报文", packet);
		Message msg;
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);
		sndPkt.acknum = packet.seqnum+sizeof(packet.payload);
		sndPkt.checksum = pUtils->calculateCheckSum(sndPkt);
		pUtils->printPacket("接收方发送确认报文", sndPkt);
		pns->sendToNetworkLayer(SENDER, sndPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
		expectSeqNum = sndPkt.acknum;
		std::cout<<"发送成功"<<std::endl;
	}else{
		if (checkSum != packet.checksum) {
			pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
		}
		else {
			pUtils->printPacket("接收方没有正确收到发送方的报文,报文序号不对", packet);
		}
		pUtils->printPacket("接收方重新发送上次的确认报文", sndPkt);
		pns->sendToNetworkLayer(SENDER, sndPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送上次的确认报文
	}
}
/*
	1.确认报文损坏丢失问题
*/