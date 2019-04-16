#include <stdio.h>
#include <sys/socket.h>     //socket
#include <netinet/in.h>     //struct sockaddr_in
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

void *deal_client(void *arg);

//并发服务器  echo  服务器收到啥 就给客户端发啥
int main(int argc, char const *argv[])
{
    if(argc != 2)
    {
        printf("PLEASE INPUT : ./demo port\n");
        return 0;
    }


    //创建tcp套接字
    int sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if(sockfd < 0)
    {
        perror("socket");
        return 0;
    }

    //绑定固定的接口
    struct sockaddr_in myAddr;
    bzero(&myAddr , sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(atoi(argv[1]));
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(sockfd , (struct sockaddr *)&myAddr , sizeof(myAddr));
    if(ret < 0)
    {
        perror("bind");
        return 0;
    }


    //3、 使用listen创建连接队列
    listen(sockfd , 15);

    //while(1)不停提取客户端
    while(1)
    {
        struct sockaddr_in cAddr;
        bzero(&cAddr , sizeof(cAddr));
        socklen_t clen = sizeof(cAddr);
        int newfd = accept(sockfd ,(struct sockaddr *)&cAddr , &clen);
        if(newfd < 0)
        {
            perror("accept");
            break;
        }

        //打印客户端的信息(非必须)
        char ip_str[16]="";
        inet_ntop (AF_INET , &cAddr.sin_addr.s_addr , ip_str , 16);
        printf("客户端%s:%hu连接了服务器\n",ip_str,ntohs(cAddr.sin_port));

        //创建一个线程 服务器客户端  
        pthread_t tid;  
        // pthread_create(&tid, NULL, 服务于客户端段的回调函数, 已连接套接字);  
        pthread_create(&tid, NULL, deal_client, (void *)newfd);  
        //线程分离  
        pthread_detach(tid);  

    }  

    close(sockfd);
    return 0;
}


void *deal_client(void *arg)
{
    //获取已连接套接字  
    int newFd = (int)arg;  

    //不停的接收客户端的信息(用户实现的地方)  
    while(1)  
    {     
        unsigned char buf[1500]="";  
        int len = recv(newFd, buf, sizeof(buf), 0);  
        if(len == 0)  
            break;  
        //给客户端回应  
        send(newFd, buf, len, 0);  
    }  

    //关闭已连接套接字  
    close(newFd);  
    return NULL;  

}

