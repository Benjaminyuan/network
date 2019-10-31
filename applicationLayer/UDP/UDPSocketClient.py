from socket import *
serverName = "localhost"
serverPort = 8008
#AF_INET表示使用IPv4底层网络
# SOCK_DGRAM表示用UDP传递数据
clientSocket = socket(AF_INET,SOCK_DGRAM)
message = input("Input message\n")
print(message)
clientSocket.sendto(message.encode(),(serverName,serverPort))
returnMessage,serverAddress = clientSocket.recvfrom(2014)
print(returnMessage.decode())
 