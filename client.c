#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h> 
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#define 	PLAY	"PLAY"
#define		PAUSE 	"PAUSE"
#define		RESUME	"RESUME"
#define 	NEXT	"NEXT"
#define 	PREV	"PREV"
#define		SUICIDE "STOP"

//we expect a deamon to have an active /tmp/musicd.sock file
//on termination of the deamon the sock file dies
//we use the sock file to understand the persence of deamon
int send_msg_soc(char* msg)
{
	
 	struct sockaddr_un soc_addr;
	int sock;
	sock=socket(AF_UNIX,SOCK_STREAM,0);
      	
	memset(&soc_addr,0,sizeof(soc_addr));
	soc_addr.sun_family=AF_UNIX;
	strncpy(soc_addr.sun_path,"/tmp/musicd.sock",sizeof(soc_addr.sun_path)-1);

	int len = offsetof(struct sockaddr_un,sun_path)+strlen(soc_addr.sun_path)+1;
	if(connect(sock,(struct sockaddr*)&soc_addr,len)==-1)
		return -1;

	if(write(sock,msg,strlen(msg))==-1)
	{
		fprintf(stderr,"write of %s to socket failed",msg);
	}
	close(sock);
	return 0;

}
void start_norm_deamon()
{
 	struct sockaddr_un soc_addr;
	int sock;
	sock=socket(AF_UNIX,SOCK_STREAM,0);
      	
	memset(&soc_addr,0,sizeof(soc_addr));
	soc_addr.sun_family=AF_UNIX;
	strncpy(soc_addr.sun_path,"/tmp/musicd.sock",sizeof(soc_addr.sun_path)-1);
	int len = offsetof(struct sockaddr_un,sun_path)+strlen(soc_addr.sun_path)+1;
	if(connect(sock,(struct sockaddr*)&soc_addr,len)==0)
	{
		write_all(sock,SUICIDE,strlen(SUICIDE));
		close(sock);
		usleep(100000);
	}
	//existing deamon killed and /tmp/musicd.sock destroyed....
	pid_t pid=fork();
	if(pid<0)
	{
		perror("error in forking process");
	       	exit(0);
	}
	if(pid==0)
	{
		execl("./norm_musicd","musicd",NULL);  //this will spawn normal deamon
	}
	else
	{
		exit(0); 	//parents exit here...
	}
}
void start_shuffle_deamon()
{
	//will be implemented
}
void start_single_play_deamon()
{
	//will be implemented
}
int main(int argc,char* argv[])
{
	int opt;
	int option_index=0;

	static struct option long_opts[]={
		{"start",no_argument,0,'S'},
		{"shuffle",no_argument,0,'h'},
		{"play",no_argument,0,'p'},
		{"pause",no_argument,0,'a'},
		{"resume",no_argument,0,'r'},
		{"stop",no_argument,0,'s'},
		{"next",no_argument,0,'n'},
		{"previous",no_argument,0,'v'},
		{0,0,0,0}
	};
	while((opt=getopt_long(argc,argv,"Shparsnv",long_opts,&option_index))!=-1)
	{
		switch(opt)
		{
			case 'S': printf("start");break;
			case 'h': printf("shuffle");break;
			case 'p': printf("play single file");start_norm_deamon();break;
			case 'a': send_msg_soc(PAUSE);break;
			case 'r': send_msg_soc(RESUME);break;
			case 's': send_msg_soc(SUICIDE);break;
			case 'n': send_msg_soc(NEXT);break;
			case 'v': send_msg_soc(PREV);break;
			default:
				  printf("unknown option");break;
		}
	}
	return 0;
}
