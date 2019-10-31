from socket import *
serverPort = 8008
serverSocket = socket(AF_INET,SOCK_DGRAM)
# 默认为localhost
serverSocket.bind(("",serverPort))
print("listen port"+str(serverPort)+'...')
while True:
    message,clientAddress = serverSocket.recvfrom(2048)
    string = message.decode()
    print(string)
    serverSocket.sendto(string.encode(),clientAddress)