#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <vector>
#include <errno.h>
#include<stdlib.h>
#include<sys/types.h>
#include<fstream>
int main(int argc, char **argv)
{
    char buff[2014] = "my name is Benji", buff2[6000];
    int n;
    struct sockaddr_in servaddr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int len = sizeof(servaddr);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(3000);
    servaddr.sin_addr.s_addr = inet_addr("39.97.228.101");
    if (connect(sockfd, (sockaddr *)&servaddr, len) < 0)
    {
        std::cout << "connect failed " << std::endl;
    }
    int in = open("./test/request",O_RDONLY,S_IRUSR);
    while ((n = read(in, buff, sizeof(buff))) > 0)
    {
        std::cout<<"string length is "<< n<< std::endl;
        if (send(sockfd, buff, n, 0) < 0)
        {
            printf("send error! %s(errno :%d)\n", strerror(errno), errno);
            break;
        }
        std::ofstream  out;
        out.open("./test/Gakki_copy.jpg",std::ios::out|std::ios::binary);
        if ((n = recv(sockfd, buff2, sizeof(buff2), 0)) > 0)
        {
            std::cout<<"echoed from server length is:"<<n << std::endl;
            out.write(buff2,n);
            std::cout<<std::endl;
        }
    }
    close(sockfd);
}