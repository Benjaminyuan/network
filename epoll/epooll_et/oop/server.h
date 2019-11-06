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
#include<fstream>
#include<vector>
#include<map>
#include<string>

#define DEFAULT_PORT 3000
#define MAXCONN 1024
using namespace std;
class HttpParser
{
    public:
        const char *buff;
        char * body;
        int content_length;
        std::string method;
        std::string url;
        std::string protocal;
        std::string base_dir;
        map<string,string> headers;
        string Get(string key);
        HttpParser(const char* raw_data);
        void parseHeader();
    private:
        void parseRequestBasicContent(const string s);
        void parseHeaderContent(vector<string> headers);
        void readData();
};
