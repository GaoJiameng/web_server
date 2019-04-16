#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

static int sockfd;

void sig_dispose(int sig)
{
    if(SIGINT == sig){
        close(sockfd);
        puts("\nclose!");
        exit(0);
    }
}
/************************************************************************
函数名称：    int main(int argc, char *argv[])
函数功能：    tftp客户端程序，可以从tftp服务器下载程序
************************************************************************/
int main(int argc, char *argv[])
{
    unsigned short p_num = 0;
    unsigned char cmd = 0;
    char cmd_buf[512] = "";
    char recv_buf[516] = "";
    struct sockaddr_in client_addr;
    socklen_t cliaddr_len = sizeof(client_addr);
    signal(SIGINT,sig_dispose);

    if( argc < 3 ){           //如果参数个数小于3,则认为输入命令错误
        printf("cmd example:./tftpc 192.168.8.8 hello.txt\n");
        return 0;
    }
    
    //TFTP基于 UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
        perror("socket error");
        exit(-1);
    }
    
    struct sockaddr_in dest_addr;
    bzero(&dest_addr, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(69);    
    inet_pton(AF_INET, argv[1], &dest_addr.sin_addr.s_addr);
    
    //构造下载请求,argv[2]为文件名
    int len = sprintf(cmd_buf, "%c%c%s%c%s%c", 0, 1, argv[2], 0, "octet", 0);    //发送读数据包请求
    sendto(sockfd, cmd_buf, len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    
    int fd = open(argv[2], O_WRONLY|O_CREAT, 0666);
    if(fd < 0 ){
        perror("open error");
        close(sockfd);
        exit(-1);
    }
    
    do{
        //接收服务器发送的内容   1   1
        len = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&client_addr, &cliaddr_len);
        
        cmd = recv_buf[1];//00 xx  
        if( cmd == 3 )    //是否为数据包
        {
            //接收的包编号是否为上次包编号+1
            if((unsigned short)(p_num+1) == ntohs(*(unsigned short*)(recv_buf+2) ))
            {
                write(fd, recv_buf+4, len-4);
                p_num = ntohs(*(unsigned short*)(recv_buf+2));
                
                printf("recv:%d\n", p_num);//十进制方式打印包编号
            }            
            recv_buf[1] = 4;
            sendto(sockfd, recv_buf, 4, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
        }
        else if( cmd == 5 ) //是否为错误应答
        {
            close(sockfd);
            close(fd);
            unlink(argv[2]);//删除文件
            printf("error:%s\n", recv_buf+4);
            exit(-1);
        }        
    }while(len == 516); //如果收到的数据小于516则认为出错
    
    close(fd);
    return 0;
}
