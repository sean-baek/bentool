#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4

void error_message(char *message);

int main(int argc, char * argv[])
{
    int sock, result, opnd_cnt;
    char opmsg[BUF_SIZE];
    struct sockaddr_in serv_addr;

    if(argc!=3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock==-1)
        error_message("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_aton(argv[1],&serv_addr.sin_addr);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if((connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) == -1)
        error_message("connect error");
    else
        printf("connected\n");

    fputs("Operand count : ",stdout);
    scanf("%d", &opnd_cnt);
    opmsg[0] = (char)opnd_cnt;
    for(int i =0;i<opnd_cnt;i++)
    {
        printf("Operand %d : ",i+1);
        scanf("%d", (int *)&opmsg[i*OPSZ+1]);
    }
    
    fgetc(stdin);
    fputs("Operator : ", stdout);
    scanf("%c", &opmsg[opnd_cnt*OPSZ+1]);
    write(sock, opmsg, opnd_cnt*OPSZ+2);

    read(sock,&result, RLT_SIZE);
    printf("Operation result %d\n", result);
    close(sock);
    return 0;
}

void error_message(char *message)
{
    fputs(message ,stderr);
    fputc('\n', stderr);
    exit(1);
}