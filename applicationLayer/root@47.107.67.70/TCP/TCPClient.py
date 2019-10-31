from socket import*
serverName= "localhost"
serverPort = 8008
ClientSocket = socket(AF_INET,SOCK_STREAM)
ClientSocket.connect((serverName,serverPort))
sentence = input("input whatever you want\n")
ClientSocket.send(sentence.encode())
recv = ClientSocket.recv(1024)
print(recv.decode())
ClientSocket.close()