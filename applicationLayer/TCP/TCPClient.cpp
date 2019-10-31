#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
//这个包引入用于解析IPv4地址
#include <arpa/inet.h>
#define PORT 8008
int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock_fd,new_sock,valread;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char * hello = "hello from the client\n";
    if((sock_fd = socket(AF_INET,SOCK_STREAM,0))<0){
        printf("\nsocket error");
    }
    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    //连接
    if(connect(sock_fd,(struct sockaddr *) &address,sizeof(address))<0){
        printf("Conection failed");
        return -1;
    }
    //发送信息
    send(sock_fd,hello,strlen(hello),0);
    printf("hello message send\n");
    //读取信息
    valread = read(sock_fd,buffer,1024);
    printf("%s\n",buffer);
    return 0;
}
