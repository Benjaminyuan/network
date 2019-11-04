#include<iostream>
#include <stdio.h>
#include <fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string>
#include<vector>
#include<map>
using namespace std;
struct http_parser
{
    std::string method;
    std::string url;
    std::string protocal;
    map<string,string> headers;
    string Get(string key){
        return headers[key];
    };
};
void parseRequestBasicContent(const string s,struct http_parser & http);
void parseHeaderContent(vector<string> headers,struct http_parser & http);

int main(){
    struct http_parser parser;
    char buff[2048];
    vector<string> header;
    int in = open("./test/request",O_RDONLY,S_IRUSR);
    int n = read(in,buff,sizeof(buff));
    if (n < 0){
        std::cout << " fail to read data"<<std::endl;
    }
    // else{
    //     std::cout<< " data length:"<< n<< "data content"<< buff<<endl;
    //     cout<<"----------------" <<endl;
    // }
    close(in);
    int i=0;
    int lineStart =0;
    string basicInfo;
    while(buff[i] && i < 2048){
        if(buff[i] =='\n'){
            int size = i-lineStart+1;
            char temp[size];
            memcpy(temp,buff+lineStart,size-1);
            string s = temp;
            if (lineStart == 0){
                basicInfo = s; 
            }else{
                header.push_back(s);
            }
            lineStart = i+1;
        }
        i++;
    }
    cout<<"i:"<< i<< endl;
    int size = i-lineStart+1;
    char temp[size];
    memcpy(temp,buff+lineStart,size-1);
    string s = temp;
    header.push_back(s);
    cout<<"header line-1:" <<basicInfo<< endl;
    parseRequestBasicContent(basicInfo,parser);
    cout<< "url:"<<parser.url<<" method: "<< parser.method << " protocal: "<< parser.protocal<<endl;
    parseHeaderContent(header,parser);
    for(map<string,string>::iterator it = parser.headers.begin();it != parser.headers.end();++it){
        cout<<"header-key: "<< it->first << " value: "<< it->second << endl;
    }
    cout<<"not exist key:" <<parser.Get("key")<< endl;
    cout<<"exist key:"<<parser.Get("Host")<<endl;
}
void parseRequestBasicContent(const string s,struct http_parser & http){
    vector<string> res;
    int last =0;
    for(int i=0;i<s.length();i++){
        if(s[i] == ' '){
            res.push_back(s.substr(last,i-last));
            last = i+1;
        }
    }
    res.push_back(s.substr(last,s.length()-last));
    if(res.size() != 3){
        cout<< "recv invalid request, parse line:"<< s<<"failed "<< endl;
        throw logic_error("invalid htpp request");
    }
    http.method = res[0];
    http.url = res[1];
    http.protocal = res[2];
}
void parseHeaderContent(vector<string> headers,struct http_parser & http){
    cout<<"parse headers start!! total "<<headers.size()<<" line"<<endl;
    for(int i=0;i<headers.size();i++){
        string  raw = headers[i];
        for(int j=0;raw.length();j++){
            if(raw[j] == ':'){
                string key = raw.substr(0,j);
                if(j+1 >= raw.length()){
                    cout<<"parse header failed,parse line"<< raw<<endl;
                    throw logic_error("parse header failed");
                }
                string value = raw.substr(j+2,raw.length()-j-2);
                http.headers[key] =value;
                break;
            }
        }
    }
    cout<<"parse headers success !! total "<<http.headers.size()<<" line"<<endl;
}