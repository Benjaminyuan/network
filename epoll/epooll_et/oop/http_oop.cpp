#include"server.h"
HttpParser::HttpParser(const char *raw_data){
   buff = raw_data;
   base_dir = "./public";
}
string HttpParser::Get(string key){
        return headers[key];
}
void HttpParser::parseHeader(){
    vector<string> header;
    int i=0;
    int lineStart =0;
    string basicInfo;
    while(buff[i] && i < 2048){
        if(buff[i] =='\n'){
            int size = i-lineStart;
            if(size == 1){
                break;
            }
            char *temp = new char[size];
            memcpy(temp,buff+lineStart,size-1);
            temp[size-1] = '\0';
            string s = temp;
            if (lineStart == 0){
                basicInfo = s;
            }else{
                header.push_back(s);
            }
            lineStart = i+1;
            delete [] temp;
        }
        i++;
    }
    cout<<"i:"<< i<< endl;
    cout<<"header line-1:" <<basicInfo<< endl;
    parseRequestBasicContent(basicInfo);
    parseHeaderContent(header);
    readData();
}
void HttpParser::parseRequestBasicContent (const string s){
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
    method = res[0];
    url = res[1];
    protocal = res[2];
}
void HttpParser::parseHeaderContent (vector<string> headerList){
     cout<<"parse headers start!! total "<<headerList.size()<<"line"<<endl;
    for(int i=0;i<headerList.size();i++){
        string  raw = headerList[i];
        cout<<"parse raw:"<< raw <<"\nraw_length:"<<raw.length() << endl;
        if(raw.length() == 1){
            break;
        }
        for(int j=0;raw.length();j++){
            if(raw[j] == ':'){
                string key = raw.substr(0,j);
                if(j+1 >= raw.length()){
                    cout<<" parse header failed, parse line"<< raw<<endl;
                    throw logic_error("parse header failed");
                }
                string value = raw.substr(j+2,raw.length()-j-2);
                headers[key] =value;
                break;
            }
        }
    }
    cout<<"parse headers success !! total "<<headers.size()<<" line"<<endl;
}

void HttpParser::readData(){
    string path = base_dir.append(url);
    std::cout<<"readDat file_path:"<<path<<std::endl;
    ifstream in ;
    in.open(path.c_str(),ios::in);
    in.seekg(0,in.end);
    content_length = in.tellg();
    in.seekg(0,in.beg);
    body = new char[content_length];
    in.read(body,content_length);
    setRes();
}
void HttpParser::setRes(){
    char temp [100];
    //添加请求信息
    sprintf(temp,"%s %s %s\r\n",protocal,"200","OK");
    std::cout<<"长度"<< strlen(temp)<< std::endl;
    res.append(temp);
    // 添加请求头部
    sprintf(temp,"%s: %s\r\n","content-type","image/jpeg");
    res.append(temp)
    sprintf(temp,"%s: %s\r\n","content-length",content_length);
    res.append(temp);
    //添加空行
    res.append("\r\n");
    //添加body
    res.append(body);
    std::cout<< "res:"<< std::endl;
    std::cout<<res.c_str()<<std::endl;
    std::cout<<"---------"<<std::endl;
}