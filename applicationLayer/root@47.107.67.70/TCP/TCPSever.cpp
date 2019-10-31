#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#define MYPORT 8008
//端口
#define BACKLOG 10
//等待连接队列长度
class TCPSever
{
  private:
    //文件描述符，unix 都是文件
    int sockfd, new_fd, valread;
    //存储套接字的相关信息
    struct sockaddr_in my_addr;
    int opt = 1;
    int addresslen = sizeof(my_addr);
    char buffer[1024] = {0};
    char *hello = "hello from the server";
    //初始化socket类型,AF_INET申明使用IPv4协议,SOCK_STREAM申明传输层使用TCP协议,0申明网络层使用IP协议
  public:
    void init()
    {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        // if((setsockopt()))
        //配置套接字相关信息
        my_addr.sin_family = AF_INET;
        my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        my_addr.sin_port = htons(MYPORT);
        //为套接字绑定信息
        if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
    }
    void listenRequest()
    {
        //开始监听请求
        listen(sockfd, BACKLOG);
        while (1)
        {
            new_fd = accept(sockfd, (struct sockaddr *)&my_addr, (socklen_t *)&addresslen);
            // if((new_fd = accept(sockfd,(struct sockaddr *)& my_addr,(socklen_t *)& addresslen))<0){
            //     perror("accept failed");
            //     exit(EXIT_FAILURE);
            // }
            if (new_fd > 0)
            {
                printf("接受请求");
                printf("%d\n", new_fd);
                valread = read(new_fd, buffer, 1024);
                printf("%s/n", buffer);
                send(new_fd, hello, strlen(hello), 0);
            }
        }
    };
};
int main(int argc, char const *argv[])
{
   TCPSever server;
   server.init();
   server.listenRequest();
   return 0;
}
