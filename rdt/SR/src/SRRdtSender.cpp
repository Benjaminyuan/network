#include "Global.h"
#include"SRRdtSender.h"

SRRdtSender::SRRdtSender():nextSeqNum(1),waitingState(false),base(1){

}
SRRdtSender::~SRRdtSender(){

}
bool SRRdtSender::getWaitingState(){
    return waitingState;
}
bool SRRdtSender::send(const Message &messages){
    // 超出窗口大小，refuse
    if(nextSeqNum >= base+ SR_N){
        return false;
    }
    Packet pkt;
    pkt.acknum = -1;
    pkt.seqnum = nextSeqNum;
    pkt.checksum = 0;
    memcpy(pkt.payload,messages.data,sizeof(messages.data));
    pkt.checksum = pUtils->calculateCheckSum(pkt);
	pUtils->printPacket("发送方发送报文", pkt);
    pns->sendToNetworkLayer(RECEIVER,pkt);
    pkt.acknum = ACK_UNRCV;
    pkt_map[nextSeqNum] = pkt;
    // 关联pkt seqnum 
    pns->startTimer(SENDER,Configuration::TIME_OUT,nextSeqNum);
    nextSeqNum++;
    return true;
}
void SRRdtSender::receive(const Packet &ackPkt){
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    // 校验和不对，返回
    if(checkSum != ackPkt.checksum){
		pUtils->printPacket("sender-recv:接收方没有正确收到发送方的报文,数据校验错误", ackPkt);
        return ;
    }
    // 窗口最前分组的确认
    if(ackPkt.acknum == base){
		pUtils->printPacket("sender-recv:接收方正确收到,窗口第一个分组确认", ackPkt);
        base = ackPkt.acknum +1;
        pkt_map.erase(ackPkt.acknum);
        pns->stopTimer(SENDER,ackPkt.acknum);
        // base移动到从窗口开始到第一个没有被确认的seqnum位置
        for(int i=ackPkt.acknum +1;i< ackPkt.acknum+pkt_map.size()+1;i++){
            if(pkt_map.count(i) != 0){
                auto iter = pkt_map.find(i);
                if(iter->second.seqnum == ACK_RCV){
                    pkt_map.erase(i);
                    base++;
                }else{
                    break;
                }
            }
        }
        std::cout << "滑动窗口移动: "<< ackPkt.acknum<< "-->"<<base<<std::endl;
    }else if(ackPkt.acknum > base && ackPkt.acknum < base+SR_N){
        if(pkt_map.count(ackPkt.acknum) == 0){
            return ;
        }
        auto iter = pkt_map.find(ackPkt.acknum);
        if(iter->second.seqnum == ACK_UNRCV){
            iter->second.seqnum = ACK_RCV;
            pns->stopTimer(SENDER,ackPkt.acknum);
        }
    }
    return ;
}
void SRRdtSender::timeoutHandler(int seqNum){
    if(pkt_map.count(seqNum) == 0){
        return ;
    }
    auto iter =pkt_map.find(seqNum);
    pns->startTimer(SENDER,Configuration::TIME_OUT,seqNum);  
    pns->sendToNetworkLayer(RECEIVER,iter->second);
    pUtils->printPacket("发送超时包:",iter->second);
    return;
}