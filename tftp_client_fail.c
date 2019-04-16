#include<stdio.h>    
#include <arpa/inet.h>//htons inet_pton    
#include <sys/socket.h>//socket    
#include <unistd.h>//close    
#include<string.h>//bzero    
#include<netinet/in.h>//struct sockaddr_in    
#include<stdlib.h>//atoi    
#include<pthread.h>//线程相关函数 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void* deal_recv_msg(void *arg);    
int main(int argc, char  *argv[])    
{    
    if(argc != 3)    
    {    
        printf("please input: ./demo IP Filename\n");    
        return 0;    
    }    

    //1、创建一个用于udp通信 套接字    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
    if(sockfd < 0)    
    {    
        perror("socket");    
        return 0;    
    }  
    
    //主线程 进行数据的发送    
    //定义一个 目的地址套接字结构    
    struct sockaddr_in dstAddr;    
    bzero(&dstAddr, sizeof(dstAddr));    
    dstAddr.sin_family = AF_INET;   

    //组下载a.txt的请求   发送给69号端口
    //将解析到的ip port 放入 目的地址结构 里面    
    dstAddr.sin_port = htons(69);    
    inet_pton(AF_INET, argv[1], &dstAddr.sin_addr.s_addr); 

    char sendCmd[1024]="";
    int len = sprintf(sendCmd , "%c%c%s%c%s%c" , 0,1, argv[2] , 0 ,"octet" , 0);


     
    sendto(sockfd , sendCmd , len , 0 , (struct sockaddr *)&dstAddr, sizeof(dstAddr));

    //打开本地址文件
    int fd = open(argv[2], O_WRONLY|O_CREAT);
     if(fd < 0)
        {
            perror("open");
            _exit(-1);
        } 

    //服务器 发送文件数据 块编号 从1开始
    int counter = 0;
    while(1)
    {   
        
        unsigned char buf[1024]="";    
        struct sockaddr_in fromAddr;    
        socklen_t addrLen = sizeof(fromAddr);
        int ret = recvfrom(sockfd , buf , sizeof(buf) , 0 , (struct sockaddr *)&fromAddr, &addrLen);
        //buf 00 03 XX XX

        if(buf[1] == 3)
        {
            write(fd , buf , ret-4);
            printf("数据包%d\n",counter+1);
            //回应服务器ACK
            //00 04 XX XX
            buf[1] = 4;
            sendto(sockfd , buf , 4 , 0 , (struct sockaddr *)&fromAddr, sizeof(fromAddr));
            counter ++;
        }
    }    
    close(fd);
    close(sockfd);
    return 0;
}
    

  
    

 
    
  

