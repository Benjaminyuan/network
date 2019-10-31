from socket import *
import sys
class SMTPClient():
    def __init__(self):
        #简单实现的DEMO
        #https://www.cnblogs.com/zjutzz/p/4298961.html
        #一定要遵循SMTP协议的格式！！！！！！
        # self.HELLOCMD = "HELO qq.com\r\n"
        # HELO 
        # MAIL FROM
        # AUTH
        # DATA
        self.__socket = socket(AF_INET,SOCK_STREAM)
    def Connect(self):
        self.__socket.connect((sys.argv[1],25))
    def Conmunicate(self):
        while 1:
            message = self.__socket.recv(2014)
            print("S:"+message.decode())
            res = input("C:")
            res += res+"\r\n"
            self.__socket.send(res.encode())
            # self.__socket.send(self.HELLOCMD.encode())
            # message = self.__socket.recv(1024)
            # print("S:"message.decode())
if __name__== "__main__":
    Client = SMTPClient()
    Client.Connect()
    Client.Conmunicate()