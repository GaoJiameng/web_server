#include<stdio.h>    
#include <arpa/inet.h>//htons inet_pton    
#include <sys/socket.h>//socket    
#include <unistd.h>//close    
#include<string.h>//bzero    
#include<netinet/in.h>//struct sockaddr_in    
#include<stdlib.h>//atoi    
#include<pthread.h>//线程相关函数    
void* deal_recv_msg(void *arg);    
int main(int argc, char  *argv[])    
{    
    //./demo port    
    if(argc != 2)    
    {    
        printf("please input: ./demo port\n");    
        return 0;    
    }    
    
    //1、创建一个用于udp通信 套接字    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);    
    if(sockfd < 0)    
    {    
        perror("socket");    
        return 0;    
    }    
    
    //2、bind固定的端口(argv[1])和IP    
    struct sockaddr_in myAddr;    
    bzero(&myAddr, sizeof(myAddr));    
    myAddr.sin_family = AF_INET;//IPv4    
    myAddr.sin_port = htons(atoi(argv[1]));//端口    
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);//绑定自己的所有IP    
    bind(sockfd, (struct sockaddr *)&myAddr, sizeof(myAddr));    
    
    //3、创建一个线程进行数据的收发    
    pthread_t tid;    
    pthread_create(&tid, NULL, deal_recv_msg, (void *)&sockfd);    
    //线程分离    
    pthread_detach(tid);    
    
    //主线程 进行数据的发送    
    //定义一个 目的地址套接字结构    
    struct sockaddr_in dstAddr;    
    bzero(&dstAddr, sizeof(dstAddr));    
    dstAddr.sin_family = AF_INET;    
    
    while(1)    
    {    
        //获取键盘输入 "正文消息"  “sayto 10.0.122.250 8000”    
        char cmd[128]="";    
        fgets(cmd,sizeof(cmd), stdin);    
        cmd[strlen(cmd)-1] = 0;//去掉回车    
    
        //判断输入的数据 是sayto 还是正文消息    
        if(strncmp(cmd, "sayto", 5) == 0)    
        {    
            //解析IP 和端口    
            char ip_str[16]="";    
            unsigned short port = 0;    
            sscanf(cmd, "sayto %s %hu", ip_str, &port);    
    
            //将解析到的ip port 放入 目的地址结构 里面    
            dstAddr.sin_port = htons(port);    
            inet_pton(AF_INET, ip_str, &dstAddr.sin_addr.s_addr);    
            continue;//继续输入数据    
        }    
    
        //发送udp数据    
        sendto(sockfd, cmd, strlen(cmd), 0, (struct sockaddr *)&dstAddr, sizeof(dstAddr));    
            
    }    
    
    close(sockfd);    
    return 0;    
}    
void* deal_recv_msg(void *arg)    
{    
    //拿到sockfd    
    int sockfd = *(int *)arg;    
    
    //不停的接受网络数据    
    while(1)    
    {    
        unsigned char msg[1024]="";    
        struct sockaddr_in fromAddr;    
        socklen_t addrLen = sizeof(fromAddr);    
        recvfrom(sockfd, msg,sizeof(msg), 0, (struct sockaddr *)&fromAddr, &addrLen);    
    
        //提取发送者的IP    
        char ip_str[16]="";    
        inet_ntop(AF_INET, &fromAddr.sin_addr.s_addr, ip_str, INET_ADDRSTRLEN);    
    
        //格式化输出    
        printf("\r[%s:%hu] %s\n", ip_str, ntohs(fromAddr.sin_port), msg);    
    }    
} 
