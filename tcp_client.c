#include <stdio.h>  
#include<sys/socket.h>//socket  
#include <netinet/in.h>//struct sockaddr_in  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <string.h>


int main(int argc, char const *argv[])  
{  
     //1、创建tcp套接字  SOCK_STREAM tcp  
     //没有端口 IP 主动链接服务器  
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);  
    if(sockfd < 0)  
     {  
         perror("socket");  
        return 0;  
    }  
   
    //2、使用connect链接服务器  
    // connect(sockfd, 服务器的IP端口, );  
    //服务器的地址结果  
    struct sockaddr_in serAddr;  
    bzero(&serAddr, sizeof(serAddr));  
    serAddr.sin_family = AF_INET;  
    serAddr.sin_port = htons(9000);//服务器的端口  
    serAddr.sin_addr.s_addr = inet_addr("10.0.122.250");//服务器的IP  
    connect(sockfd, (struct sockaddr *)&serAddr, sizeof(serAddr));  
  
    //使用send给服务器发个请求 i am tcp client  
    char msg[]="你打球像蔡徐坤";  
    send(sockfd, msg, strlen(msg), 0);  
  
    //使用recv接受服务器的应答(带阻塞)  
    unsigned char buf[512]="";  
    recv(sockfd, buf, sizeof(buf), 0);  
    printf("server data:%s\n", buf);  
  
    //关闭套接字  
    close(sockfd);  
    return 0;  
}