#include <stdio.h> 
    #include <stdlib.h> 
    #include <unistd.h>
    #include <strings.h>
    #include <sys/types.h> 
    #include <sys/socket.h> 
    #include <netinet/in.h> 
    #include <netdb.h>     
    #include<string.h>

    #define PORT 1137  
    #define MAXDATASIZE 100   

    char sendbuf[1024];
    char sendbuf1[1024];
    char recvbuf[1024];
    char name[10];
    char name1[10];
    int fd;

    void pthread_recv(void* ptr)
{
    while(1)
    {
    if ((recv(fd,recvbuf,MAXDATASIZE,0)) == -1){ 
        printf("recv() error\n"); 
        exit(1); 
        } 
    printf("%s",recvbuf);
    memset(recvbuf,0,sizeof(recvbuf));
    }
}



    int main(int argc, char *argv[]) 
    { 
    int  numbytes;   
    char buf[MAXDATASIZE];   
    struct hostent *he;       
    struct sockaddr_in server;  


    if (argc !=2) {         printf("填入服务器ip地址\n"); 
    exit(1); 
    } 


    if ((he=gethostbyname(argv[1]))==NULL){  /*如果用的是域名则转化为ip形式*/
    printf("gethostbyname() error\n"); 
    exit(1); 
    } 

    if ((fd=socket(AF_INET, SOCK_STREAM, 0))==-1){ /*建立套接字*/ 
    printf("socket() error\n"); 
    exit(1); 
    } 

    bzero(&server,sizeof(server));

    server.sin_family = AF_INET; 
    server.sin_port = htons(PORT); 
    server.sin_addr = *((struct in_addr *)he->h_addr);  
    if(connect(fd, (struct sockaddr *)&server,sizeof(struct sockaddr))==-1){  
    printf("connect() error\n");/*连接失败*/ 
    exit(1); 
    } 

    printf("连接成功\n");
    char str[]="我已进入聊天\n";
    printf("请注册一个用户名：");
    fgets(name,sizeof(name),stdin);
    pthread_t tid;
    pthread_create(&tid,NULL,pthread_recv,NULL);/*创建线程，避免接收和发送相互卡住*/

    while(1)
    {
        memset(sendbuf,0,sizeof(sendbuf));/*清空上一次留下的数据*/     memset(sendbuf1,0,sizeof(sendbuf1)); 
        fgets(sendbuf1,sizeof(sendbuf1),stdin);
        if(strcmp(sendbuf1,"exit")==0){
            memset(sendbuf1,0,sizeof(sendbuf1));
            printf("您已退出群聊\n");
            send(fd,sendbuf1,(strlen(sendbuf1)),0);
            break;
        }
       send(fd,name,strlen(name)-1,0);
       send(fd,":",1,0);
        send(fd,sendbuf1,(strlen(sendbuf1)),0);
        
    }
 

    close(fd);  
 }
