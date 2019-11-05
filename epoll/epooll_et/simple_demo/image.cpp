#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<iostream>
#include<fstream>
using namespace std;
int main(){
    ifstream in ;
    in.open("./test/Gakki.jpg",ios::in);
    in.seekg(0,in.end);
    int length = in.tellg();
    in.seekg(0,in.beg);
    char * buff = new char[length];
    in.read(buff,length);
    ofstream out ;
    out.open("./test/Gakki_copy.jpg",ios::out|ios::binary);
    out.write(buff,length);
    in.close();
    out.close();
    delete [] buff;
}