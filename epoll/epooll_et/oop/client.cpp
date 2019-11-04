#include"./server.h"
int main(int argc, char **argv)
{
    char buff[2014] = "my name is Benji", buff2[1024];
    int n;
    struct sockaddr_in servaddr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int len = sizeof(servaddr);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(3000);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(sockfd, (sockaddr *)&servaddr, len) < 0)
    {
        std::cout << "connect failed " << std::endl;
    }
    int in = open("../test/request",O_RDONLY,S_IRUSR);
    while ((n = read(in, buff, sizeof(buff))) > 0)
    {
        std::cout<<"string length is "<< n<< std::endl;
        if (send(sockfd, buff, n, 0) < 0)
        {
            printf("send error! %s(errno :%d)\n", strerror(errno), errno);
            break;
        }
        if ((n = recv(sockfd, buff2, sizeof(buff2), 0)) > 0)
        {
            std::cout<<"echoed from server length is:"<<n << std::endl;
            write(0, buff2, n);
            std::cout<<std::endl;
        }
    }
    close(sockfd);
}