#include <stdio.h>  
#include<sys/socket.h>           //socket  
#include<unistd.h>               //close  
#include<netinet/in.h>           //struct sockaddr_in  
#include<string.h>               //strlen bzero  
#include<arpa/inet.h>            //htons  inet_pton

int main(int argc,char *argv[])
{
    while(1)
    {
        //创建一个用于UDP通信的套接字sockfd  
        //AF_INET IPv4  AF_INET6 IPv6  
        //SOCK_DGRAM udp编程     
        //0 自动识别协议类型 
        int sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
        if(sockfd<0)
        {
            perror("socket");
            return 0;
        }
        printf("sockfd = %d\n",sockfd);
        //使用sendto  10.0.122.200   8080 发送udp数据  
        //定义一个ipv4套接字地址结构  
        struct sockaddr_in dstAddr;
        //先将dstAddr清0  
        //memset(&dstAddr, 0, sizeof(dstAddr));  
        bzero (&dstAddr , sizeof(dstAddr));
        //赋值协议
        dstAddr.sin_family = AF_INET;//IPv4
        //赋值 目的端口 8080 
        dstAddr.sin_port = htons(8000);
        //赋值 目的IP 10.0.122.200
        inet_pton(AF_INET , "10.0.122.250" , &dstAddr.sin_addr.s_addr);

        char msg[] = "I'm 55kai, I'm not cheat!";  
        //如果 sockfd没有认为指定端口 ip 那么在第一次调用sendto的时候系统会自动添加随机端口 和自身IP  
        sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&dstAddr, sizeof(dstAddr));  
    
        //关闭套接字  
        close(sockfd);  
        return 0;
    }
    
}
