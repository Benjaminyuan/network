from socket import *
import threading
class Proxy():
    def __init__(self,config):
        self.socket = socket(AF_INET,SOCK_STREAM)
        self.socket.bind((config["HOST_NAME"],config["BIND_PORT"]))
        self.socket.listen(10)
        self.__client = {}
        self.ClientNum = 0
        self.config = config
    def StartListen(self):
        while True:
            connect,address = self.socket.accept()
            thread = threading.Thread(name=self.__getClientName(address),target=self.ProxyItem,args=(connect,address))
            thread.start()
    def ProxyItem(self,conn,address):
        recv = conn.recv(self.config["MAX_REQUEST_LEN"])
        req = recv.decode()
        print(req)
        print(len(req))
    def __getClientName(self,address):
        self.__client[self.ClientNum]=address
        self.ClientNum +=1 
        return address
if  __name__=="__main__":
    config = {"HOST_NAME":"localhost","BIND_PORT":8008,"MAX_REQUEST_LEN":2048}
    Proxy = Proxy(config)
    Proxy.StartListen()