#include <stdio.h>  
#include<sys/socket.h>//socket  
#include <netinet/in.h>//struct sockaddr_in  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <string.h>  
  
int main(int argc, char const *argv[])  
{  
    //1、创建TCP套接字(监听套接字) 获得客户端连接请求  
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);  
    if(sockfd < 0)  
    {  
        perror("socket");  
        return 0;  
    }  

    //2、bind绑定固定的端口 ip  以便客户端连接  
    struct sockaddr_in myAddr;  
    bzero(&myAddr, sizeof(myAddr));  
    myAddr.sin_family = AF_INET;  
    myAddr.sin_port = htons(8080);  
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);  
    bind(sockfd, (struct sockaddr *)&myAddr, sizeof(myAddr));  

    //3、listen创建连接队列  
    listen(sockfd, 15);  
   
    //4、accept从连接队列中 提取已完成的连接 产生一个与具体客户端通信的套接字  
    while(1)  
    {  
        //accept 阻塞 一次只能提取一个客户端 返回值 代表和该客户端通信的套接字  
        struct sockaddr_in cAddr;  
        socklen_t cLen = sizeof(cAddr);  

        //newFd 才能和客户端通信  
        int newFd = accept(sockfd, (struct sockaddr *)&cAddr, &cLen);  
        if(newFd < 0)  
            break;  
        char ip_str[16]="";  
        inet_ntop(AF_INET, &cAddr.sin_addr.s_addr, ip_str, 16);  
        printf("%s:%hu已连接了服务器\n", ip_str, ntohs(cAddr.sin_port));  
            
        //5、使用send recv 发送或接受数据  
        //获取客户端的请求  
        unsigned char msg[512]="";  
        int len = recv(newFd, msg, sizeof(msg), 0);  

        //应答客户端 ECHO服务器  
        send(newFd, msg, len, 0);  
        close(newFd);  
    }  
        
       
    //6、关闭所有套接字  
    close(sockfd);  
    return 0;  
 }  
