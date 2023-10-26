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

#define N 32

#define R 1   
#define L 2  
#define Q 3  
#define H 4  
int n;
/*定义通信双方的信息*/
typedef struct {
    int type;
    char name[N];
    char data[256];
}MSG;

void do_register(int sockfd,MSG *msg)
{
    printf("注册账户\n");
    printf("------------\n");
    msg->type = R;
    printf("请输入用户名: ");
    scanf("%s",msg->name);
    getchar();
    printf("请输入密码: ");
    scanf("%s",msg->data);
    if(send(sockfd,msg,sizeof(MSG),0) <0)
    {
        printf("send msg error\n");
        exit(1);
    }
    if(recv(sockfd,msg,sizeof(MSG),0) <0)
    {
        printf("recv msg error\n");
        exit(1);
    }
    printf("---------------------\n");
    printf("--------  ");
    printf("%s\n",msg->data);
    printf("---------------------\n");
    return;
}
int do_login(int sockfd,MSG *msg)
{
    printf("登陆账户\n");
    printf("--------------\n");
    msg->type = L;
    printf("请输入用户名: ");
    scanf("%s",msg->name);
    getchar();
    printf("请输入密码: ");
    scanf("%s",msg->data);
    if(send(sockfd,msg,sizeof(MSG),0) <0)
    {
        printf("send msg error\n");
        return -1;
    }
    if(recv(sockfd,msg,sizeof(MSG),0) <0)
    {
        printf("recv msg error\n");
        return -1;
    }
    printf("---------------\n");
    if(strncmp(msg->data,"OK",3) == 0)
    {
        return 1;
    }
    return -1;
}
int do_query(int sockfd,MSG *msg)
{
    printf("查询单词\n");
    return 0;
}
int do_history(int sockfd,MSG *msg)
{
    printf("查询\n");
    return 0;
}

int main(int argc, char const *argv[])
{
    int sockfd;
    struct sockaddr_in serveraddr;
    MSG msg;
    if(argc != 3)
    {
        printf("./client <serverip> <port>\n");
        exit(1);
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

    if(connect(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)) < 0)
    {
        perror("connect");
        exit(1);
    }
    while(1)
    {
        printf("请输入指令\n");
        printf("1、注册  2、登陆  3、退出 \n");
        printf("---------------------- \n");
        scanf("%d",&n);
        getchar();
        switch(n)
        {
            case 1:
                do_register(sockfd,&msg);
                break;
            case 2:
               if(do_login(sockfd,&msg) ==1)
               {
                printf("登陆成功\n");
                printf("---------------\n");
                    goto next;
               }
               else{
                printf("登陆失败\n");
                printf("---------------\n");
               }break;
            case 3:
                close(sockfd);
                exit(1);
                break;
            default:
                printf("cmd error\n");
        }
    }
next:
    while(1)
    {
        printf("1、查询单词 2、历史记录查询 3、退出\n");
        scanf("%d",&n);
        switch (n)
        {
        case 1:
            do_query(sockfd,&msg);
            break;
        case 2:
            do_history(sockfd,&msg);
            break;
        case 3:
            close(sockfd);
            exit(1);
            break;
        default:
          printf("cmd error\n");
        }
    }
    return 0;

}
