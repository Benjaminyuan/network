import threading
class msg():
    def __init__(self,message):
        self.message = message
class message():
    def __init__(self,seqnum,acknum,playload):
        self.seqnum = seqnum
        self.acknum = acknum
        self.playload = playload
class A_sender():
    def __init__(self,msg):
        self.nowSendSeqnum = 0
        self.status = "unsend"
    def rdt_send(self,message):
        
    def start_timer(self,time):
        pass
    def stop_timer(self):
        pass
    def check_sum(self):
        pass
