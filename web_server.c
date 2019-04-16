#include <stdio.h>  
#include<sys/socket.h>//socket  
#include <netinet/in.h>//struct sockaddr_in  
#include <arpa/inet.h>  //htons  inet_pton
#include <unistd.h>  
#include <string.h> 
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void *deal_client(void *fd);

int main(int argc, char const *argv[])
{
    if(argc != 2)
    {
        printf("Input Demo: ./demo port");
        return 0;
    }
    //1、创建TCP套接字(监听套接字) 获得客户端连接请求  
    int sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if(sockfd < 0)
    {
        perror("socket");
        return 0;
    }


    int yes = 1;
    setsockopt(sockfd, SOL_SOCKET ,SO_REUSEADDR, &yes , sizeof(yes));
    //2、bind绑定固定的端口 ip  以便客户端连接
    struct sockaddr_in myAddr;
    //初始化 myAddr
    bzero(&myAddr , sizeof(myAddr));
    //复制本机协议
    myAddr.sin_family = AF_INET;
    //复制本机port
    myAddr.sin_port = htons(atoi(argv[1]));
    //复制本机IP
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);


    int ret = bind(sockfd , (struct sockaddr *)&myAddr , sizeof(myAddr));
    // if(ret < 0)
    // {
    //     perror("bind");
    //     return 0;
    // }

    //3、listen创建连接队列 
    listen(sockfd , 15);

    //4、accept从连接队列中 提取已完成的连接 产生一个与具体客户端通信的套接字 
    while(1)
    {
        struct sockaddr_in cAddr;
        socklen_t cLen = sizeof(cAddr);
        bzero(&cAddr , cLen);
        int newfd = accept(sockfd , (struct sockaddr *)&cAddr , &cLen);
        if(newfd < 0)
        {
            perror("accept");
            break;
        }
        //打印客户端的信息(非必须)
        char ip_str[16]="";
        inet_ntop(AF_INET , &cAddr.sin_addr.s_addr , ip_str , 16);
        printf("客户端%s:%hu连接了服务器\n",ip_str , ntohs(cAddr.sin_port));

        //创建一个线程 服务器客户端 
        pthread_t tid;
        // pthread_create(&tid, NULL, 服务于客户端段的回调函数, 已连接套接字);  
        pthread_create(&tid , NULL , deal_client , (void *)&newfd);
        //线程分离  
        pthread_detach(tid);
    }
    close(sockfd);
    return 0;
}

void *deal_client(void *fd)
{
    //获取已连接套接字  
    int newFd = *(int *)fd;  

    char err[]=	"HTTP/1.1 404 Not Found\r\n"		\
		"Content-Type: text/html\r\n"		\
		"\r\n"								\
		"<HTML><BODY>File not found</BODY></HTML>";

    char head[]="HTTP/1.1 200 OK\r\n"					\
			"Content-Type: text/html\r\n"		\
			"\r\n";

    //不停的接收客户端的信息(用户实现的地方)  
    //while(1)  
    // {     
        //获取浏览器请求recv(buf)
        unsigned char buf[1500]=""; 
        
        // char *buf_src[3]; 
        char path[256] = "./html/";
        int len = recv(newFd, buf, sizeof(buf), 0);  
          

        sscanf(buf,"GET /%s", path+7);
        if(path[7] == 0)
        {
            strcpy(path , "./html/index.html");
        }
        // printf("buf_src[2] = %s\n",buf_src[2]);
        //解析数据 提取 浏览器需要的文件名FileName
        
        
        //sprintf(path, "./html/%s", buf_src[2]);
        printf("path=%s\n",path);

        int fd_path = open(path, O_RDONLY);
        if(fd_path < 0)
        {
            perror("open");
            send(newFd, err, strlen(err), 0);
            _exit(-1);
        } 
        else
        {
            send(newFd, head, strlen(head), 0);
            while(1)
            {
                unsigned char buf_file[1024]="";
                int len_file = read(fd_path , buf_file ,sizeof(buf_file));
                send(newFd , buf_file , len_file , 0);
                if(len_file < 1024)
                    break;
            }
            close(fd_path);
        }

    // }  

    //关闭已连接套接字  
    close(newFd);  
    return NULL;  
}

