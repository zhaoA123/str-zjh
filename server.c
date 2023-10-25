#include <strings.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sqlite3.h>
#include <unistd.h>
#include <signal.h>
#define N 32

#define R 1 
#define L 2  
#define Q 3  
#define H 4  

#define DATABASE "my.db"

/*定义通信双方的信息*/
typedef struct {
    int type;
    char name[N];
    char data[256];
}MSG;

int do_client(int acceptfd, sqlite3 *db);
void do_register(int sockfd,MSG *msg,sqlite3 *db);
int do_history(int sockfd,MSG *msg,sqlite3 *db);
int do_query(int sockfd,MSG *msg,sqlite3 *db);
int do_login(int sockfd,MSG *msg,sqlite3 *db);

int main(int argc, char const *argv[])
{
    int sockfd;
    struct sockaddr_in serveraddr;
    MSG msg;
    sqlite3 *db;
    int acceptfd;
    pid_t pid;
    if(argc != 3)
    {
        printf("./client <serverip> <port>\n");
        exit(1);
    }
    
    if(sqlite3_open(DATABASE,&db) != SQLITE_OK)
    {
        printf("sqlite3_open");
        exit(1);
    }
    else{
        printf("数据库已打开\n");
    }
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        perror("socket");
        exit(1);  
    }
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
    serveraddr.sin_port = htons(atoi(argv[2]));

    if(bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
    {
        perror("bind");
        exit(1);
    }
    if(listen(sockfd,5) < 0)
    {
        perror("listen");
        exit(1);
    }
    signal(SIGCHLD,SIG_IGN);
    while(1)
    {
        if((acceptfd = accept(sockfd,NULL,NULL)) < 0 )
        {
                perror("new");
                return -1;
        }
        if((pid = fork()) < 0)
        {
            perror("fork");
            return -1;
        }
        else if(pid == 0)
        {
            printf("123\n");
            close(sockfd);
            do_client(acceptfd,db);
        }
        else
        {
            close(acceptfd);
        }
    }
    return 0;

}
int do_client(int acceptfd, sqlite3 *db)
{
    MSG msg;
    while(recv(acceptfd,&msg,sizeof(msg),0) > 0)
    {
        printf("type:%d\n",msg.type);
        switch (msg.type)
        {
        case R:
             do_register(acceptfd,&msg,db);
             break;
        case L:
             do_login(acceptfd,&msg,db);
             break;
        case Q:
             do_query(acceptfd,&msg,db);
             break;
        case H:
             do_history(acceptfd,&msg,db);
             break;
        default:
            printf("cmd error\n");
        }
    }
    close(acceptfd);
    exit(1);
    return 0;
}

void do_register(int sockfd,MSG *msg,sqlite3 *db)
{
    char *errmsg;
    char sql[350];
    if((sprintf(sql,"insert into usr values('%s', %s);",msg->name,msg->data))<0)
    {exit(1);}
    printf("%s\n",sql);
  if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
  {
    strcpy(msg->data,"用户名已存在");
  }
  else
  {
    printf("client register ok\n");
    strcpy(msg->data,"用户名符合要求");
  }
  if(send(sockfd,msg,sizeof(MSG),0) <0)
    {
        printf("send msg error\n");
        exit(1);
    }
}
int do_login(int sockfd,MSG *msg,sqlite3 *db)
{
    char *errmsg;
    char sql[350];
    int nrow;
    int ncloumn;
    char **resulp;
     if((sprintf(sql,"select * from usr where name = '%s' and pass = '%s';",msg->name,msg->data))<0)
    {exit(1);}
    if(sqlite3_get_table(db,sql,&resulp,&nrow,&ncloumn,&errmsg) != SQLITE_OK)
    {
        return -1;
    }
    if(nrow) strcpy(msg->data,"OK");
    else  strcpy(msg->data,"NO");
    if(send(sockfd,msg,sizeof(MSG),0) <0)
    {
        printf("send msg error\n");
        exit(1);
    }
    return 0;
}
int do_query(int sockfd,MSG *msg,sqlite3 *db)
{
    printf("查询单词\n");
    return 0;
}
int do_history(int sockfd,MSG *msg,sqlite3 *db)
{
    printf("查询记录\n");
    return 0;
}
