#include "server.h"
HttpParser::HttpParser(int& epoll,int fd)
{
    base_dir = "./public";
    clientfd = fd;
    epoll_id = &epoll;
    body = NULL;
    buff = new char[BUFFSIZE];
}
HttpParser::HttpParser(){
    base_dir = "./public";
    buff = new char[BUFFSIZE];
}
int HttpParser::recvData(){
    return recv(clientfd,buff,BUFFSIZE,0);
}
string HttpParser::Get(string key)
{
    return headers[key];
}
void HttpParser::parseRequestBasicContent(const string s)
{
    vector<string> res;
    int last = 0;
    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] == ' ')
        {
            res.push_back(s.substr(last, i - last));
            last = i + 1;
        }
    }
    res.push_back(s.substr(last, s.length() - last));
    if (res.size() != 3)
    {
        cout << "recv invalid request, parse line:" << s << "failed " << endl;
        throw logic_error("invalid htpp request");
    }
    method = res[0];
    url = res[1];
    protocal = res[2];
}
void HttpParser::parseHeaderContent(vector<string> headerList)
{
    for (int i = 0; i < headerList.size(); i++)
    {
        string raw = headerList[i];
        if (raw.length() == 1)
        {
            break;
        }
        for (int j = 0; raw.length(); j++)
        {
            if (raw[j] == ':')
            {
                string key = raw.substr(0, j);
                if (j + 1 >= raw.length())
                {
                    cout << " parse header failed, parse line" << raw << endl;
                    throw logic_error("parse header failed");
                }
                string value = raw.substr(j + 2, raw.length() - j - 2);
                headers[key] = value;
                break;
            }
        }
    }
    cout << "parse headers success !! total " << headers.size() << " line" << endl;
}
void HttpParser::parseHeader()
{
    vector<string> header;
    int i = 0;
    int lineStart = 0;
    string basicInfo;
    while (buff[i] && i < 2048)
    {
        if (buff[i] == '\n')
        {
            int size = i - lineStart;
            if (size == 1)
            {
                break;
            }
            char *temp = new char[size];
            memcpy(temp, buff + lineStart, size - 1);
            temp[size - 1] = '\0';
            string s = temp;
            if (lineStart == 0)
            {
                basicInfo = s;
            }
            else
            {
                header.push_back(s);
            }
            lineStart = i + 1;
            delete[] temp;
        }
        i++;
    }
    cout << "header line-1:" << basicInfo << endl;
    parseRequestBasicContent(basicInfo);
    parseHeaderContent(header);
    readData();
}

void HttpParser::readData()
{
    string path = base_dir+url;
    std::cout << "readDat file_path:" << path << std::endl;
    ifstream in(path.c_str(), ios::in);
    if (!in)
    {
        std::cout << "file not exit " << std::endl;
        send_body = false;
    }else{
        in.seekg(0, in.end);
        content_length = in.tellg();
        in.seekg(0, in.beg);
        body = new char[content_length];
        in.read(body, content_length);
        send_body = true;
        std::cout << "read data finish " << "content_length"<<content_length<<std::endl;
        in.close();
    }
     // EpollOpt(EPOLL_CTL_MOD, fd, EPOLLOUT);

    epoll_event clientfd_event;
    clientfd_event.data.fd = clientfd;
    clientfd_event.events = EPOLLOUT | EPOLLET ;
    //| EPOLLONESHOT;
    if (epoll_ctl(*epoll_id, EPOLL_CTL_MOD, clientfd, &clientfd_event) == -1)
    {
        std::cout << "epoll fail to add events" << std::endl;
        throw std::logic_error("epoll fail to add events");
    }
    std::cout<<"thread-"<< std::this_thread::get_id()<<":"<<"read finish"<< std::endl;
}
void HttpParser::sendRes()
{
    int sum=0;
    char temp[100];
    std::string res = "";
    std::cout << "-----------------" << std::endl;
    std::cout << "url:" << url << " method: " << method << " protocal: " << protocal << std::endl;
    std::cout<<"thread-"<< std::this_thread::get_id()<<":"<< std::endl;
    std::cout << "-------write return data-------" << std::endl;
    if (send_body)
    {
        //添加请求信息
        sprintf(temp, "%s %d %s\r\n", protocal.c_str(), 200, "OK");
        std::cout << "长度" << strlen(temp) << std::endl;
        res.append(temp);
        // 添加请求头部
        sprintf(temp, "%s: %s\r\n", "content-type", "image/jpeg");
        res.append(temp);
        sprintf(temp, "%s: %d\r\n", "content-length", content_length);
        res.append(temp);
        //添加空行
        res.append("\r\n");
        //添加body
        std::cout << "header-length:" << res.length() << std::endl;
        std::cout << "header:" << std::endl;
        std::cout << res.c_str() << std::endl;
        std::cout << "---------write return data with body finish--------" << std::endl;
        int m = send(clientfd,res.c_str(),res.length(),0);
        int n = send(clientfd,body,content_length,0);
        if(body != NULL ){
            delete [] body;
            body = NULL;
        } 
        sum = m +n;
    }
    else
    {   
        std::cout << "-------write return data-------" << std::endl;
        sprintf(temp, "%s %d %s\r\n", protocal.c_str(), 404, "Not Found");
        res.append(temp);
        res.append("\r\n");
        res.append("file you want not exist!!");
        std::cout << "---------write return data finish--------" << std::endl;
        sum = send(clientfd,res.c_str(),res.length(),0);
    }
    if (sum == 0)
    {
            if (epoll_ctl(*epoll_id, EPOLL_CTL_DEL, clientfd, NULL) != -1)
            {
                std::cout << "client disconnected,clientfd:" << clientfd << std::endl;
            }
            close(clientfd);
    }
    else
    {
            std::cout << "send data to :"<<clientfd<<", finished : " << sum << std::endl;
            // send(fd,parser.body,parser.content_length,0);
            close(clientfd);
    }
}
void HttpParser::finishRequest()
{
    std::cout << "free data" << std::endl;
    content_length = 0;
}