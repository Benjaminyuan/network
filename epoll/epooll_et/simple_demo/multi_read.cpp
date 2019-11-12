
#include <iostream>
#include<fstream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include<pthread.h>
class Test {
    public:
        void read_data(char *buff);
};
void Test::read_data(char * buff){
    std::ifstream in("./test/multi_test",std::ios::in);
    if(!in){
        std::cout << "file not exist" << std::endl;
    }
    buff = new char[200];
    in.seekg(0,in.end);
    int content_length = in.tellg();
    in.seekg(0,in.beg);
    in.read(buff,content_length);
    std::cout<<"thread-"<< std::this_thread::get_id()<<":"<<buff<< std::endl;
}
int main(int argc, const char** argv) {
    char* buff[3];
    std::thread* threads[3];
    Test test[3];
    for(int i=0;i<3;i++){
       threads[i] = (new std::thread(&Test::read_data,&test[i],buff[i]));
    }
    for(int i=0;i<3;i++){
        threads[i]->join();
    }
    return 0;
}