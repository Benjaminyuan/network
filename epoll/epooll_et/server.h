#include <sys/epoll.h>
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
#include<vector>
#include<map>
#include<string>

#define DEFAULT_PORT 8888
#define MAXCONN 1024
enum class RequestMethod{GET,POST,PUT,OTHER};
// struct request{
//     char buff[2048];
//     map<string,string> headers;
//     RequestMethod method;
//     int max;
//     int read_pos;
//     int fd;
// };
// class HttpServer{
//     public:
//         HttpServer()
//         HttpServer(int port);
//         CreateEpoll();
//         Listen();
//     private:
//         int epollfd;
//         int listenfd;
//         struct sockaddr_in bind_addr;
//         epoll_event epoll_events[2048];
//         // request
//         map<int,*request> request_map;
//         map<string,RequestMethod> method_map;
//         //解析http
//         ParseHttp(int fd);
// }