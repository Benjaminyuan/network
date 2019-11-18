#include <sys/epoll.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include<dirent.h>
#include <sys/socket.h>
#include<sys/stat.h>
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
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include<pthread.h>
#include<unordered_map>
#define DEFAULT_PORT 3000
#define MAXCONN 1024
#define BUFFSIZE 2048

using namespace std;

class HttpParser
{
    public:
        char *buff;
        char *body;
        bool send_body;
        int content_length;
        int  clientfd;
        const int *epoll_id;
        std::string method;
        std::string url;
        std::string protocal;
        std::string base_dir;
        map<string,string> headers;
        HttpParser();
        string Get(string key);
        HttpParser(int& epoll,int fd);
        void parseHeader();
        void finishRequest();
        int recvData();
        void sendRes();
    private:
        void parseRequestBasicContent(const string s);
        void parseHeaderContent(vector<string> headers);
        void readData();
        void printHeaders();
        void tryGetContentType();
};

