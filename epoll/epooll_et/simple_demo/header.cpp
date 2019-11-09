#include<iostream>
#include<string>

int main(){
    std::string res_header;
    char temp [100];
    //添加请求信息
    sprintf(temp,"%s %s %s\r\n","HTTP/1.1","200","OK");
    std::cout<<"长度"<< strlen(temp)<< std::endl;
    res_header.append(temp);
    // 添加请求头部
    sprintf(temp,"%s: %s\r\n","content-type","image/jpeg");
    res_header.append(temp);
    sprintf(temp,"%s: %s\r\n","content-length","1024");
    res_header.append(temp);
    //添加请求头部
    res_header.append("\r\n");
    res_header.append("{\"key\":\"value\"}");
    std::cout<< "--------"<<std::endl;
    std::cout<<res_header<<std::endl;
    std::cout<< "--------"<<std::endl;
}