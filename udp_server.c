#include <stdio.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <unistd.h>  
#include <string.h>  
#include <arpa/inet.h>  
int main(int argc, char const *argv[])  
{  
    //创建用于通信的udp套接字  
    int sockfd = 0;  
    //socket创建的套接字 没有端口和IP  
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);  
    if(sockfd < 0)  
    {  
        perror("socket");  
        return 0;  
    }  
   while(1)
    {
        //使用bind 给sockfd绑定一个固定的ip和端口  
        struct sockaddr_in myAddr;  
        bzero(&myAddr,sizeof(myAddr));  
        myAddr.sin_family = AF_INET;  
        myAddr.sin_port = htons(8080);  
        //bind只能绑定自己的IP  
        myAddr.sin_addr.s_addr = htonl(INADDR_ANY);//INADDR_ANY 0 通配地址  
        
        bind(sockfd, (struct sockaddr *)&myAddr, sizeof(myAddr));      
    
    
        //接收udp消息  
        struct sockaddr_in fromAddr;  
        socklen_t fromLen = sizeof(fromAddr);  
        unsigned char buf[128]="";  
        int len = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&fromAddr, &fromLen);  
    
        //哪个IP给我发的:fromAddr.sin_addr.s_addr  需要转换成 点分十进制数串  
        char ip_str[16]="";  
        inet_ntop(AF_INET, &fromAddr.sin_addr.s_addr, ip_str, INET_ADDRSTRLEN);  
        printf("发送者IP:%s 端口:%hu\n", ip_str, ntohs(fromAddr.sin_port) );  
    
        printf("len = %d\n", len);  
        printf("buf = %s\n", buf);  
    
    
        close(sockfd);  
    }
    return 0;  
}  
