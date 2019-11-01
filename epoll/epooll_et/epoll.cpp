#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <errno.h>
#include <sys/types.h>

char buffData[2024][2024];
int main(int argc, const char **argv)
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        std::cout << "create listen socket error" << std::endl;
        return -1;
    }
    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    //设置socket 为非阻塞
    int oldSocketFlag = fcntl(listenfd, F_GETFL, 0);
    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
    if (fcntl(listenfd, F_SETFL, newSocketFlag) == -1)
    {
        close(listenfd);
        std::cout << "set listenfd to noblock failed" << std::endl;
        return -1;
    }
    struct sockaddr_in bind_addr;
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind_addr.sin_port = htons(3000);
    if (bind(listenfd, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) == -1)
    {
        close(listenfd);
        std::cout << "bind listen socket failed " << std::endl;
        return -1;
    }
    if (listen(listenfd, SOMAXCONN) == -1)
    {
        std::cout << "listen error." << std::endl;
        close(listenfd);
        return -1;
    }
    int epollfd = epoll_create(1);
    if (epollfd == -1)
    {
        std::cout << " create epoll failed error." << std::endl;
        close(listenfd);
        return -1;
    }
    epoll_event listen_fd_event;
    listen_fd_event.data.fd = listenfd;
    listen_fd_event.events = EPOLLIN;
    // ET模式需要增加的代码
    //listen_fd_event.events |= EPOLLET;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &listen_fd_event) == -1)
    {
        std::cout << "epoll_ctl error" << std::endl;
        close(listenfd);
        return -1;
    }
    int n;
    while (true)
    {
        epoll_event epoll_events[2024];
        // params: fd,happened events,max_events,time_out
        n = epoll_wait(epollfd, epoll_events, 2014, 1000);
        if (n < 0)
        {
            //errno 是记录系统的最后一次错误代码
            // Interrupted system call,系统调用中断
            if (errno == EINTR)
            {
                continue;
            }
            break;
        }
        else if (n == 0)
        {
            //1000ms内没有请求
            continue;
        }
        for (int i = 0; i < n; i++)
        {
            if (epoll_events[i].events & EPOLLIN)
            {
                if (epoll_events[i].data.fd == listenfd)
                {
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
                    int clientfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_addr_len);
                    if (clientfd != -1)
                    {
                        //设置为非阻塞
                        int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
                        int newSocketFlag = oldSocketFlag | O_NONBLOCK;
                        if (fcntl(clientfd, F_SETFD, newSocketFlag) == -1)
                        {
                            close(clientfd);
                            std::cout << "set clientfd to nonblocking failed." << std::endl;
                        }
                        else
                        {
                            epoll_event clientfd_event;
                            clientfd_event.events = EPOLLIN;
                            clientfd_event.data.fd = clientfd;
                            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &clientfd_event) != -1)
                            {
                                std::cout << "new client accepted,clientfd:" << clientfd << std::endl;
                            }
                            else
                            {
                                std::cout << "add client fd to epollfd failed " << std::endl;
                                close(clientfd);
                            }
                        }
                    }
                }
                else
                {
                    std::cout << "client fd: " << epoll_events[i].data.fd << "recv data" << std::endl;
                    int fd = epoll_events[i].data.fd;
                    char* ch = buffData[fd];
                    int m = recv(epoll_events[i].data.fd,ch,1024, 0);
                    if (m == 0)
                    {
                        if (epoll_ctl(epollfd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL) != -1)
                        {
                            std::cout << "client disconnected,clientfd:" << epoll_events[i].data.fd<< std::endl;
                        }
                        close(epoll_events[i].data.fd);
                    }
                    else if (m < 0)
                    {
                        if (errno != EWOULDBLOCK && errno != EINTR)
                        {
                            if (epoll_ctl(epollfd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL) != -1)
                            {
                                std::cout << "client disconnected,clientfd:" << epoll_events[i].data.fd << std::endl;
                            }
                            close(epoll_events[i].data.fd);
                        }
                    }
                    else
                    {
                        std::cout << "recv data from client:" << epoll_events[i].data.fd << ",data:" << ch << std::endl;
                        epoll_event clientfd_event;
                        clientfd_event.events = EPOLLOUT;
                        clientfd_event.data.fd = epoll_events[i].data.fd;
                        epoll_ctl(epollfd,EPOLL_CTL_MOD,epoll_events[i].data.fd,&clientfd_event);
                    }
                }
            }
            else if (epoll_events[i].events & EPOLLOUT)
            {
               int m =  send(epoll_events[i].data.fd,buffData[i],1024,0);
               if(m == 0)
               {
                    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL) != -1)
                    {
                        std::cout << "client disconnected,clientfd:" << epoll_events[i].data.fd<< std::endl;
                    }
                    close(epoll_events[i].data.fd);
               }
               else
               {
                    epoll_event clientfd_event;
                    clientfd_event.events = EPOLLIN;
                    clientfd_event.data.fd = epoll_events[i].data.fd;
                    epoll_ctl(epollfd,EPOLL_CTL_MOD,epoll_events[i].data.fd,&clientfd_event);
                }
            }
        }
    }
    close(listenfd);
    return 0;
}


