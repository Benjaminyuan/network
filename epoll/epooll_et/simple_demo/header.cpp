#include<iostream>
#include<fstream>
#include<string>
using namespace std;
int main(){
    int content_length;
    char *body;
    string path = "./test/Gakki.jpg";
    std::cout<<"readDat file_path:"<<path<<std::endl;
    ifstream in ;
    ofstream out;
    in.open(path.c_str(),ios::in);
    in.seekg(0,in.end);
    content_length = in.tellg();
    in.seekg(0,in.beg);
    body = new char[content_length];
    in.read(body,content_length);
    std::string res_header;
    char temp [100];
    //添加请求信息
    sprintf(temp,"%s %s %s\r\n","HTTP/1.1","200","OK");
    std::cout<<"长度"<< strlen(temp)<< std::endl;
    res_header.append(temp);
    // 添加请求头部
    sprintf(temp,"%s: %s\r\n","content-type","image/jpeg");
    res_header.append(temp);
    sprintf(temp,"%s: %d\r\n","content-length",content_length);
    res_header.append(temp);
    //添加请求头部
    res_header.append("\r\n");
    std::cout<< "body:"<< body<<std::endl;
    out.open("./test/Gakki_test.jpg",ios::out);
    out.write(body,content_length);
    std::cout<< "--------"<<std::endl;
    std::cout<<res_header<<std::endl;
    std::cout<< "--------"<<std::endl;
}