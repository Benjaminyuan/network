
#include<stdio.h>
#include<string.h>
#include<unistd.h>
 
int main()
{
    char szBuf[128];
    char szPath[128];
    
    memset(szBuf, 0x00, sizeof( szBuf));    
    memset( szPath, 0x00, sizeof(szPath));
 
    getcwd(szBuf, sizeof(szBuf)-1);
    printf("buf:%s\n", szBuf);
 
    int ret =  readlink("/proc/self/exe", szPath, sizeof(szPath)-1 );
    printf("ret:%d\n", ret);
    printf("path:%s\n", szPath);
 
    return 0;
 
}
