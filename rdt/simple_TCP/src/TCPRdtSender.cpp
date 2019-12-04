#include "Global.h"
#include"TCPRdtSender.h"

TCPRdtSender::TCPRdtSender():nextSeqNum(1),base(1),timer_status(false),accumulate_sum(0){

}
TCPRdtSender::~TCPRdtSender(){

}
bool TCPRdtSender::getWaitingState(){
    return false;
}

bool TCPRdtSender::send(const Message &messages){
    // 超出窗口大小，refuse
    if(after(nextSeqNum,base+TCP_N)){
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
    // 没有定时器打开
    if(timer_status == false){
        // GNB不要关联pkt seqnum 所以设定一个不会出现的seqnum
        pns->startTimer(SENDER,Configuration::TIME_OUT,-1);
        timer_status = true;
    }
    nextSeqNum += sizeof(messages.data) ;
    return true;
}
void TCPRdtSender::receive(const Packet &ackPkt){
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    // 校验和不对，返回
    if(checkSum != ackPkt.checksum){
		pUtils->printPacket("sender-recv:接收方没有正确收到发送方的报文,数据校验错误", ackPkt);
        return ;
    }
    if(after(ackPkt.acknum,base)){
        std::cout << "滑动窗口移动: "<< base << "--->"<< ackPkt.acknum << std::endl;
		base = ackPkt.acknum;
        if(timer_status == true) {
            pns->stopTimer(SENDER,-1);
        }
        //累计确认
        while (pkt_queue.size() !=0 && before(pkt_queue.front().seqnum , base))
        {
            pkt_queue.pop_front();
        }
        //还有未被确认的包
        if(pkt_queue.size() != 0){
            pns->startTimer(SENDER,Configuration::TIME_OUT,-1);
            timer_status = true;
        }
    }
    if(ackPkt.acknum == base){
        accumulate_sum++;
        // 三次冗余ACK 重传
        if(accumulate_sum == 3){
            auto pkt = pkt_queue.front();
            pUtils->printPacket("超时重传报文", pkt);
            pns->sendToNetworkLayer(RECEIVER,pkt);
            accumulate_sum = 0;
        }
    }
}
void TCPRdtSender::timeoutHandler(int seqNum){
    if(pkt_queue.size() != 0 ){
        auto pkt = pkt_queue.front();
        pns->sendToNetworkLayer(RECEIVER,pkt);
        pns->startTimer(SENDER,Configuration::TIME_OUT,-1);
        timer_status = true;
    }
    return;
}