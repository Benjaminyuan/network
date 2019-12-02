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
    if(nextSeqNum >= base+ GBN_N){
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
    pkt_queue.push_back(pkt);
    if(base == nextSeqNum){
        // GNB不要关联pkt seqnum 所以设定一个不会出现的seqnum
        pns->startTimer(SENDER,Configuration::TIME_OUT,-1);
    }
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
    // 接收到正常的ACk，将pkt移出处队列,不正常的直接忽略
    if(ackPkt.acknum >= pkt_queue.front().seqnum){
		pUtils->printPacket("sender-recv:接收方正确收到", ackPkt);
        base = ackPkt.acknum +1;
        int pos = 0;
        for(int i=0;i<pkt_queue.size();i++){
            if(pkt_queue[i].seqnum == ackPkt.acknum){
                pos = i;
                break;
            }
        }
        int j=0;
        while(j<=pos && j<pkt_queue.size()){
            pkt_queue.pop_front();
            j++;
        }
        if(base == nextSeqNum){
            //没有需要确认的包了
            pns->stopTimer(SENDER,-1);
        }else{
             //重置timer
            pns->stopTimer(SENDER,-1);
            pns->startTimer(SENDER,Configuration::TIME_OUT,-1);
        }
    }
   
}
void SRRdtSender::timeoutHandler(int seqNum){
    pns->startTimer(SENDER,Configuration::TIME_OUT,-1);
    for(auto iter = pkt_queue.begin();iter !=pkt_queue.end();iter++){
        if(iter == pkt_queue.begin()){
        	pUtils->printPacket("发送方定时器时间到，重发上次发送的报文",*iter);
        }
        pns->sendToNetworkLayer(RECEIVER,*iter);
    }
    return;
}