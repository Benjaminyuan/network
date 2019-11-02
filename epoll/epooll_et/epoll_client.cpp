#include<stdio.h>
#include <unistd.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<iostream>

int main(int argc,char** argv)
{
    struct sockaddr_in servaddr;
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    int len = sizeof(servaddr);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family =AF_INET;
    servaddr.sin_port = htons(3000);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(connect(sockfd,(sockaddr *)&servaddr,len)<0){
        std::cout << "connect failed " << std::endl;
    }
    for(;;){
        char data[] = "testest";
        char buff[1024];
        send(sockfd,data,strlen(data),0);
        recv(sockfd,buff,1024,0);
        std::cout<<"recv data:"<< buff<<std::endl;
    }
    close(sockfd);
}