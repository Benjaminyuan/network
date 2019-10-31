from socket import *
serverPort = 8008
serverSocket =socket(AF_INET,SOCK_STREAM)
serverSocket.bind(("",serverPort))
serverSocket.listen(2)
while 1:
    connectSocket,clientAddress = serverSocket.accept()
    print("connection establish")
    sentence = connectSocket.recv(1024)
    print(clientAddress)
    print(sentence.decode())
    connectSocket.send(sentence.decode().upper().encode())
    connectSocket.close()
