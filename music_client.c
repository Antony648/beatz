#include <stdlib.h>
#include <stdio.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#define  PATH   "/tmp/musicd.sock"
int main()
{
        struct sockaddr_un socket1;
        int client_fd=socket(AF_UNIX,SOCK_STREAM,0);
        if(client_fd<0)
        {
            perror("failed in opening client fd");exit(1);
        }
        memset(&socket1,0,sizeof(socket1));
        socket1.sun_family=AF_UNIX;
        strncpy(socket1.sun_path,PATH,sizeof(socket1.sun_path)-1);
        if(connect(client_fd,(struct sockaddr*)&socket1,sizeof(socket1))<0)
        {
            perror("failed to connect");exit(1);
        }
        printf("connection to deamon established with client_fd:%d\n",client_fd);
        char buffer[250]="demo";
        while(1)
        {
            printf(">");
            scanf("%s",buffer);
            int bytes=write(client_fd,buffer,sizeof(buffer));
            if(bytes<0)
            {
                perror("write to server failed");exit(1);
            }
            bytes=read(client_fd,buffer,sizeof(buffer));
            if(bytes<0)
            {
                perror("reading of message from server failed");exit(1);
            }
            buffer[bytes]='\0';
            printf("reply from server:%s\n",buffer);
        }
        return 0;
}
