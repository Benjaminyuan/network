from socket import *
class UDPping():
    def __init__(self):
        self.socket = socket(AF_INET,SOCK_DGRAM)
    def Start(self,PORT,ADDRESS):
        self.socket.bind((ADDRESS,PORT))
        while 1:
            connectSocket,address = self.socket.recvfrom(2048)
            self.socket.sendto("recieve".encode(),address)
if __name__=="__main__":
    Ping = UDPping()
    Ping.Start(8008,"localhost")
    

