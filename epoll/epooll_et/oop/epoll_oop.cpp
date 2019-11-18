#include "./server.h"
#include "threadpool.hpp"
class HttpServer
{
public:
    HttpServer(int num,int port);
    int EpollOpt(int opt, int listenfd, int event);
    int CreateEpoll();
    int Listen();

private:
    int epollfd;
    int listenfd;
    int thread_pool_num;
    std::map<int, HttpParser*> httpParsers;
    char buffData[2024][2024];
    struct sockaddr_in bind_addr;
    epoll_event epoll_events[2048];
};
// HttpServer::HttpServer() : HttpServer(DEFAULT_PORT)
// {
// }
HttpServer::HttpServer(int num,int port)
{
    thread_pool_num = num;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        std::cout << "create listen socket error" << std::endl;
    }
    int on = 1;
    //允许地址复用
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    //设置socket 为非阻塞
    int oldSocketFlag = fcntl(listenfd, F_GETFL, 0);
    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
    if (fcntl(listenfd, F_SETFL, newSocketFlag) == -1)
    {
        close(listenfd);
        throw std::logic_error("fail to set listenfd unblock!!");
    }
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(port);
    bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) == -1)
    {
        close(listenfd);
        throw std::logic_error("fail to bind listenfd!!");
    }
}
//创建epoll 实例
int HttpServer::CreateEpoll()
{
    epollfd = epoll_create(1);
    if (epollfd == -1)
    {
        std::cout << "fail to create epollfd" << std::endl;
        return -1;
    }
    return 0;
}
int HttpServer::EpollOpt(int opt, int listenfd, int event)
{
    epoll_event clientfd_event;
    clientfd_event.data.fd = listenfd;
    clientfd_event.events = event;
    if (epoll_ctl(epollfd, opt, listenfd, &clientfd_event) == -1)
    {
        std::cout << "epoll fail to add events" << std::endl;
        return -1;
    }
    return 0;
}
int HttpServer::Listen()
{
    if (listen(listenfd, MAXCONN) == -1)
    {
        std::cout << "listen error" << std::endl;
        close(listenfd);
        return -1;
    }
    if (EpollOpt(EPOLL_CTL_ADD, listenfd, EPOLLIN) == -1)
    {
        std::cout << "fail to create epoll" << std::endl;
        close(listenfd);
        return -1;
    }
    std::cout<<"init thread_pool, size: "<< thread_pool_num<< std::endl;
    ThreadPool thread_pool(thread_pool_num);
    thread_pool.start();
    while (true)
    {
        int n = epoll_wait(epollfd, epoll_events, 2048, 100);
        if (n < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            break;
        }
        else if (n == 0)
        {
            continue;
        }
        // std::vector<std::thread> threads;
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
                        int oldSocketFlag = fcntl(clientfd, F_SETFL, 0);
                        int newSocketFlag = oldSocketFlag | O_NONBLOCK;
                        if (fcntl(clientfd, F_SETFL, newSocketFlag) == -1)
                        {
                            close(clientfd);
                            std::cout << "set clientfd to nonblocking failed." << std::endl;
                            return -1;
                        }
                        else
                        {
                            if (EpollOpt(EPOLL_CTL_ADD, clientfd, EPOLLIN | EPOLLET) == 0)
                            {
                                std::cout << "new client accept,listenfd: " << clientfd << std::endl;
                            }
                            else
                            {
                                std::cout << "add client failed " << std::endl;
                                return -1;
                            }
                        }
                    }
                }
                else
                {
                    int fd = epoll_events[i].data.fd;
                    struct sockaddr_in sa;
                    socklen_t len = sizeof(sa);
                    getpeername(fd, (struct sockaddr *)&sa, &len);
                    const char *ip = inet_ntoa(sa.sin_addr);
                    int port = int(ntohs(sa.sin_port));
                    std::cout << "------------------------" << std::endl;
                    std::cout<<"clientfd: "<<fd<<" EPOLLIN"<<std::endl;
                    std::cout << "client_ip:" << ip << "\nport:" << port << std::endl;
                    std::cout << "------------------------\n"<< std::endl;
                    char *ch = buffData[fd];
                    HttpParser *parser;
                    if(httpParsers.count(fd)> 0){
                        map<int,HttpParser*>::iterator it = httpParsers.find(fd);
                        parser = (it->second);
                    }else{
                        parser = new HttpParser(epollfd,fd);
                    }
                    // std::cout << "\n\n----------
                    thread_pool.appendTask(std::bind(&HttpParser::parseHeader,parser));
                    // threads.push_back(thread(&HttpParser::parseHeader,parser));
                    httpParsers.insert(std::make_pair(fd, parser));
                    
                }
            }
            else if ((epoll_events[i].events & EPOLLOUT) && epoll_events[i].data.fd != listenfd)
            {
                int fd = epoll_events[i].data.fd;
                std::cout << "------------------------" << std::endl;
                std::cout<<"clientfd: "<<fd<<" EPOLLOUT"<<std::endl;
                std::map<int, HttpParser*>::iterator it = httpParsers.find(fd);
                if (it == httpParsers.end())
                {
                    std::cout << "fail to get parser" << std::endl;
                }
                HttpParser* parser = it->second;
                thread_pool.appendTask(std::bind(&HttpParser::sendRes,parser));
                // threads.push_back(std::thread(&HttpParser::sendRes,parser));
            }
        }
        // for(auto iter = threads.begin();iter != threads.end();iter++){
        //     iter->join();
        // }
    }
    thread_pool.stop();
    close(listenfd);
    return 0;
}
int main(int argsc,char* argv[])
{
    if(argsc != 2){
        std::cout<<"invalid params"<< std::endl;
    }
    int thread_pool_num = std::atoi(argv[1]);
    HttpServer server(thread_pool_num,DEFAULT_PORT);
    server.CreateEpoll();
    server.Listen();
}