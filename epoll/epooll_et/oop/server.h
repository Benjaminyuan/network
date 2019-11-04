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

#define DEFAULT_PORT 3000
#define MAXCONN 1024
// enum class RequestMethod{GET,POST,PUT,OTHER};
// #include<iostream>
// #include <stdio.h>
// #include <fcntl.h>
// #include<sys/types.h>
// #include<sys/stat.h>
// #include<unistd.h>
// #include<string>
// #include<vector>
// #include<map>
// class HttpParser
// {
//     public:
//         char buff[4048];
//         std::string method;
//         std::string url;
//         std::string protocal;
//         map<string,string> headers;
//         string Get(string key);
//         HttpParser(int fd);
//         void parseHeader();
//     private:
//         void parseRequestBasicContent(const string s);
//         void parseHeaderContent(vector<string> headers);
// };