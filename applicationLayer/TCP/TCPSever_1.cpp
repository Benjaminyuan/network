#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#define PORT 8008
#define backlog 10
#define IP "127.0.0.1"
#define MAXSIZE 2048
#define MAXEVENTS 10
#define BACKLOG 10
int bind_port(const char *ip, int port);
void do_epoll(int listenfd);
void handle_accept(int epollfd, int listenfd);
void do_read(int epollfd, int fd, char *buffer);
void do_wirte(int epollfd, int fd, char *buf);
void add_event(int epollfd, int op, int fd, int state);
int main(int argc, char const *argv[])
{
    int listenfd;
    listenfd = bind_port(IP,PORT);
    do_epoll(listenfd);
    return 0;
}
int bind_port(const char *ip, int port)
{
    int listenfd;
    struct sockaddr_in serverAddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == 0)
    {
        printf("socket failed\n");
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    serverAddr.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr *) & serverAddr, sizeof(serverAddr)) < 0)
    {
        printf("bind failed\n");
        exit(1);
    };
    return listenfd;
}
void do_epoll(int listenfd)
{
    int epollfd;
    struct epoll_event epoll_events[20];
    char buf[MAXSIZE];
    int ret;
    int fd;
    int readyEvents;
    struct epoll_event event;
    int i = 0;
    socklen_t addr_len;
    listen(listenfd, BACKLOG);
    epollfd = epoll_create(1);
    if (epollfd < 0)
    {
        printf("create failed\n");
        exit(1);
    }
    event.events = EPOLLIN;
    event.data.fd = listenfd;
    ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event);
    if (ret == -1)
    {
        printf("failed\n");
        exit(1);
    }
    printf("start listening\n");
    for (;;)
    {
        readyEvents = epoll_wait(epollfd, epoll_events, MAXEVENTS, -1);
        if(readyEvents>=1){
            printf("eventsNum:%d\n",readyEvents);
        }
        for (i = 0; i < readyEvents; i++)
        {
            fd = epoll_events[i].data.fd ;
            if (fd == listenfd && epoll_events[i].events == EPOLLIN)
            {
                printf("handle accept \n");
                handle_accept(epollfd, listenfd);
            }
            else if (epoll_events[i].events == EPOLLIN)
            {
                printf("handle do read  \n");                
                do_read(epollfd, fd, buf);
            }
            else if (epoll_events[i].events == EPOLLOUT)
            {
                printf("handle do write \n");
                do_wirte(epollfd, fd, buf);
            }
        }
    }
}
void handle_accept(int epollfd, int listenfd)
{
    int clientfd;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;
    clientfd = accept(listenfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientfd == -1)
    {
        printf("accept failed\n");
    }
    else
    {
        printf("New client:%s\n port:%d \n", inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);
        add_event(epollfd, EPOLL_CTL_ADD,clientfd, EPOLLOUT);
    }
}
void do_read(int epollfd, int fd, char *buf)
{
    int readNum;
    readNum = read(fd, buf, MAXSIZE);
    printf("readNum:%d\n",readNum);
    if (readNum == -1)
    { 
        printf("read error\n");
        close(fd);
            exit(1);
    }
    else if (readNum == 0)
    {
        close(fd);
        printf("client close \n");
    }
    else
    {
        printf("readMessage:%s\n", buf);
        //
        add_event(epollfd, EPOLL_CTL_MOD, fd, EPOLLOUT);
    }
}
void add_event(int epollfd, int op, int fd, int state)
{
    struct epoll_event event;
    event.events = state;
    event.data.fd = fd;
    epoll_ctl(epollfd, op, fd, &event);
}
void do_wirte(int epollfd, int fd, char *buf)
{
    int writeNum;
    char *b = "hello from the server\n";
    strcpy(buf,b);
    printf("send message:%s\n",buf);
    writeNum = write(fd, buf, strlen(buf));
    if (writeNum == -1)
    {
        printf("write error\n");
        close(fd);
        add_event(epollfd, EPOLL_CTL_DEL, fd, EPOLLOUT);
    }
    else
    {
        add_event(epollfd, EPOLL_CTL_MOD,fd, EPOLLIN);
    }
    memset(buf, 0, MAXSIZE);
}