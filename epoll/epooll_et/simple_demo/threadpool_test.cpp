#include<iostream>
#include<chrono>
#include"threadpool.cpp"
void func1(){
    std::cout << "working in thread:"<<std::this_thread::get_id() << std::endl;
}
void func2(int x){
    std::cout<<"task" << x<< "working in thread"<< std::this_thread::get_id()<<std::endl;
}
int main(int argc, const char** argv) {
    ThreadPool thread_pool(3);
    thread_pool.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    for (int i = 0; i < 6; i++)
    {
        thread_pool.appendTask(std::bind(func2,i));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    thread_pool.stop();
    return 0;
}
