from socket import *
import sys
import time
class Ping():
    def __init__(self):
        self.socket = socket(AF_INET,SOCK_DGRAM)
    def Start(self):
        self.__times = 10
        port = sys.argv[1]
        for i in range(self.__times):
            self.socket.sendto('reqest'.encode(),("localhost",int(port)))
            start = time.time()
            rep = self.socket.recv(2048)
            end = time.time()
            print(end-start)
if __name__=="__main__":
    Ping = Ping()
    Ping.Start()

