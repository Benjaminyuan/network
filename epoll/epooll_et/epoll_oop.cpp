#include "./server.h"
// method_map.insert("POST",RequestMethod::POST);
// method_map.insert("GET",RequestMethod::GET);
// method_map.insert("PUT",RequestMethod::PUT);
struct request
{
    char buff[2048];
    map<string, string> headers;
    RequestMethod method;
    int max;
    int read_pos;
    int fd;
};
class HttpServer
{
public:
    HttpServer();
    HttpServer(int port);
    int EpollOpt(int opt,int listenfd,int event);
    int CreateEpoll();
    int Listen();

private:
    int epollfd;
    int listenfd;
    struct sockaddr_in bind_addr;
    epoll_event epoll_events[2048];
    // request
    map<int, *request> request_map;
    map<string, RequestMethod> method_map;
    //解析http
    ParseHttp(int fd);
} HttpServer::HttpServer() : HttpServer(DEFAULT_PORT)
{
}
HttpServer::HttpServer(int port)
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
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
    bind_addr.sin_family = AF_INET
                               bind_addr.sin_port = htons(port);
    bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (sockaddr *)bind_addr, sizeof(bind_addr)) == -1)
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
int HttpServer::EpollOpt(int opt,int listenfd,int event)
{
    epoll_event clientfd_event;
    clientfd_event.data.fd = listenfd;
    clientfd_event.event = event;
    if(epoll_ctl(epollfd,opt,listenfd,&clientfd_event) == -1)
    {
        std::cout<<"epoll fail to add events"<<std::endl;
        return -1;
    }
    return 0;
}
int HttpServer::Listen()
{
    if (listen(listenfd, MAXCONN) == -1)
    {
        std::cout::"listen error" << std::endl;
        close(listenfd);
        return -1;
    }
    if ( EpollOpt(EPOLL_CTL_ADD,listenfd,EPOLLIN)== -1)
    {
        std::cout << "fail to create epoll" << std::endl;
        close(listenfd);
        return -1;
    }
    while(true){
        n = epoll_wait(epollfd,epoll_events,2048,100);
        if(n<0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            break;
        }
        else if (n == 0)
        {
            continue;
        }
        for(int i =0;i<n;i++)
        {
            if(epoll_events[i].events & EPOLLIN)
            {
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(listenfd);
                int clientfd = accept(listenfd,(sockaddr*)sockaddr_in,&client_addr_len);
                if (clientfd != -1)
                {
                    int oldSocketFlag = fcntl(clientfd,F_SETFL,0);
                    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
                    if(fcntl(clientfd,F_SETFL,newSocketFlag) == -1)
                    {
                        close(clientfd);
                        std::cout<< "set clientfd to nonblocking failed."<< std::endl;
                        return -1;
                    }
                    else
                    {
                        EpollOpt(EPOLL_CTL_ADD,clientfd,EPOLLIN);
                    }
                    
                }
            }
        }
    }
}