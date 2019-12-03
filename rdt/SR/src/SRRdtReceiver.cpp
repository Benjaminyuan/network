#include "Global.h"
#include"SRRdtRecevier.h"

SRRdtReceiver::SRRdtReceiver():recv_base(1){
	sndPkt.acknum = 0;
	sndPkt.checksum = 0;
	sndPkt.seqnum = -1;
	for(int i = 0; i < Configuration::PAYLOAD_SIZE;i++){
		sndPkt.payload[i] = '.';
	}
	sndPkt.checksum = pUtils->calculateCheckSum(sndPkt);
}
SRRdtReceiver::~SRRdtReceiver(){

}
void SRRdtReceiver::receive(const Packet &packet){
	int checkSum = pUtils->calculateCheckSum(packet);
	//正确收到 包
	 // 校验和不对，返回
    if(checkSum != packet.checksum){
		pUtils->printPacket("sender-recv:接收方没有正确收到发送方的报文,数据校验错误", packet);
        return ;
    }
    // 窗口最前分组的确认
    if(packet.seqnum == recv_base){
		pUtils->printPacket("sender-recv:接收方正确收到", packet);
        recv_base = packet.seqnum+1;
		Message msg;
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);
        // base移动到从窗口开始到第一个没有被确认的seqnum位置
		for(int i = packet.seqnum+1;i<packet.seqnum+SR_N;i++){
			if(recv_map.count(i) != 0){
				auto iter = recv_map.find(i);
				Message msg;
				memcpy(msg.data, iter->second.payload, sizeof(iter->second.payload));
				pns->delivertoAppLayer(RECEIVER, msg);
				recv_map.erase(i);
				recv_base++;
			}else{
				break;
			}
		}
		sndPkt.acknum = packet.seqnum;
		sndPkt.checksum = pUtils->calculateCheckSum(sndPkt);
		pUtils->printPacket("接收方发送确认报文", sndPkt);
		pns->sendToNetworkLayer(SENDER, sndPkt);
    }else if(packet.seqnum > recv_base && packet.seqnum < recv_base+SR_N){
		if(recv_map.count(packet.seqnum) == 0){
			recv_map[packet.seqnum] = packet ;
		}	
		sndPkt.acknum = packet.seqnum;
		sndPkt.checksum = pUtils->calculateCheckSum(sndPkt);
		pUtils->printPacket("接收方发送确认报文", sndPkt);
		pns->sendToNetworkLayer(SENDER, sndPkt);
    }else if(packet.seqnum < recv_base && packet.seqnum >= recv_base - SR_N){
		sndPkt.acknum = packet.seqnum;
		sndPkt.checksum = pUtils->calculateCheckSum(sndPkt);
		pUtils->printPacket("接收方发送确认报文", sndPkt);
		pns->sendToNetworkLayer(SENDER, sndPkt);
	}
    return ;
}
/*
	1.确认报文损坏丢失问题
*/