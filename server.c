   #include <stdio.h>          
    #include <strings.h>         
    #include <unistd.h>          
    #include <sys/types.h> 
    #include <sys/socket.h> 
    #include <netinet/in.h> 
    #include <arpa/inet.h>
    #include<pthread.h> 
    #include<stdlib.h>

    #define PORT 1137    
    #define BACKLOG 1 
    #define Max 5 /*must numbers of client*/
    #define MAXSIZE 1024

/*定义全局变量*/
int fdt[Max]={0};/*套接字符可被每个线程访问*/
char mes[1024];/*存储客户端发来的消息让每个线程都能访问*/ 

/**/

void *pthread_service(void* sfd)
{
    int fd=*(int *)sfd;
    while(1)
    {
        int numbytes;
        int i;
        numbytes=recv(fd,mes,MAXSIZE,0);
        if(numbytes<=0){/*接收失败清空相应的fdt[i]*/  
            for(i=0;i<Max;i++){
                if(fd==fdt[i]){
                    fdt[i]=0;             
                }
            }
            printf("numbytes=%d\n",numbytes);
            printf("exit! fd=%d\n",fd);
            break;

        }
        printf("从 %d接收到%d字符\n",fd,numbytes);
        SendToClient(fd,mes,numbytes);
        bzero(mes,MAXSIZE);

    }
    close(fd);

}


/**/
int SendToClient(int fd,char* buf,int Size)/*转发消息到其他每个客户端*/
{
    int i;
    for(i=0;i<Max;i++){
        printf("\t%d",fdt[i]);
        if((fdt[i]!=0)&&(fdt[i]!=fd)){/*转发给不是自己的其他客户端*/ 
            send(fdt[i],buf,Size,0); 
            printf("发送消息给 %d\n",fdt[i]);
        }
    }   
    return 0;


}

int  main()  
{ 


    int listenfd, connectfd;  /*初始化*/  
    struct sockaddr_in server; 
    struct sockaddr_in client;      
    int sin_size; 
    sin_size=sizeof(struct sockaddr_in); 
    int number=0;
    int fd;


    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {   
        perror("Creating socket failed.");
        exit(1);
    }


    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));/*设置端口复用*/ 

    bzero(&server,sizeof(server));  


    server.sin_family=AF_INET; 
    server.sin_port=htons(PORT); 
    server.sin_addr.s_addr = htonl (INADDR_ANY); 


    if (bind(listenfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) { 
    perror("Bind error.");
    exit(1); 
    }   


    if(listen(listenfd,BACKLOG) == -1){  
    perror("listen() error\n"); 
    exit(1); 
    } 
    printf("Waiting for client....\n");


    while(1)
    {

        if ((fd = accept(listenfd,(struct sockaddr *)&client,&sin_size))==-1) {
        perror("accept() error\n"); 
        exit(1); 

        }

        if(number>=Max){
            printf("no more client is allowed\n");
            close(fd);
        }

        int i;
        for(i=0;i<Max;i++){/*避免套接字覆盖*/ 
            if(fdt[i]==0){
                fdt[i]=fd;
                break;
            }

        }



        pthread_t tid;
        pthread_create(&tid,NULL,(void*)pthread_service,&fd);/*每来一个客户端连接请求创建一个线程*/ 

        number=number+1;/*记录客户端数量*/ 


    }


    close(listenfd);            
    }
