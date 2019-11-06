#include"server.h"
HttpParser::HttpParser(const char *raw_data){
   buff = raw_data;
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
    memcpy(temp,buff+lineStart,size);
    string s = temp;
    header.push_back(s);
    cout<<"header line-1:" <<basicInfo<< endl;
    parseRequestBasicContent(basicInfo);
    parseHeaderContent(header);
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
     cout<<"parse headers start!! total "<<headerList.size()<<" line"<<endl;
    for(int i=0;i<headerList.size();i++){
        string  raw = headerList[i];
        for(int j=0;raw.length();j++){
            if(raw[j] == ':'){
                string key = raw.substr(0,j);
                if(j+1 >= raw.length()){
                    cout<<"parse header failed,parse line"<< raw<<endl;
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

