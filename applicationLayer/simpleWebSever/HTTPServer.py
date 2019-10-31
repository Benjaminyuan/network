import re
from socket import *
# port = 8008
# serverSocket = socket(AF_INET,SOCK_STREAM)
# serverSocket.bind(("",port))
# serverSocket.listen(2)
# while 1:
#     connectSock,address = serverSocket.accept()
#     print(connectSock)
#     req = connectSock.recv(2048)
#     print(req.decode())
#     HTTPHeaders = re.split(r'\r\n',req.decode())
#     print(HTTPHeaders)
#     #注意HTTP协议的报文请求,相应格式
#     connectSock.sendall('''HTTP/1.1 200 OK\r\n\r\n
#     <html>
#   <head>
#     <title>Build A Web Server</title>
#   </head>
#   <body>
#     Hello World, this is a very simple HTML document.
#   </body>
# </html>
#      '''.encode())
#     connectSock.close()
class HTTPServer():
  def __init__(self):
    self.Socket = socket(AF_INET,SOCK_STREAM)
  def Start(self,PORT,ADDRESS):
    self.Socket.bind((ADDRESS,PORT))
    self.Socket.listen(2)
    while 1:
      self.ConnectSocket,address= self.Socket.accept()
      self.recv = self.ConnectSocket.recv(2048)
      self.HandleRequest()
  def HandleRequest(self):
      print(self.recv.decode())
      HTTPContent = re.split(r'\r\n',self.recv.decode())
      print(HTTPContent)
      fileName = re.match(r'\w+ (/[^ ]*)',HTTPContent[0]).group(1)
      print(fileName)
      file_exist = True
      if fileName =='/':
            fileName = '/index.htm'
      try:
        file = open('.'+fileName)
      except IOError:
        RESPON_START_LINE = "HTTP/1.1 404 Not Found\r\n"
        RESPON_HEADERS = "SEVER：BENJI'S SERVER\r\n\r\n"
        RESPON_BODY = "FILE NOT EXTST"
        file_exist = False
      if file_exist: 
        file_data =file.read()
        file.close()
        RESPON_START_LINE ="HTTP/1.1 200 OK\r\n"
        RESPON_HEADERS = "Server:BENJI'S SERVER\r\n\r\n"
        RESPON_BODY = file_data
        print(RESPON_BODY)
      sendBytes =RESPON_START_LINE+RESPON_HEADERS+RESPON_BODY
      self.ConnectSocket.send(sendBytes.encode())
if __name__=="__main__":
  HTTPServer = HTTPServer()
  HTTPServer.Start(8008,'localhost')