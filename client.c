#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#define 	PLAY	1
#define		PAUSE 	0
#define		RESUME	2
#define 	NEXT	3
#define 	PREV	4
#define		SUICIDE 5

struct sockaddr_un soc_addr;
int sock;
//we expect a deamon to have an active /tmp/musicd.sock file
//on termination of the deamon the sock file dies
//we use the sock file to understand the persence of deamon
void send_msg_soc(char* msg)
{
	
	if(connect(sock,(struct sockaddr_un)&soc_addr,sizeof(soc_addr))==-1)
		return -1;
	write(sock,msg,strlen(msg));

}
void start_norm_deamon()
{
	while(connect(sock,(struct sockaddr_un)&soc_addr,sizeof(soc_addr))!==0)
	{
		write(sock,"stop",4);
		usleep(50000);
	}
	//existing deamon killed and /tmp/musicd.sock destroyed....
	pid_t pid=fork();
	if(pid==0)
		execl("./norm_musicd","musicd",NULL);  //this will spawn normal deamon
	else
		exit(0); 	//parents exit here...
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
	sock=socket(AF_UINX,SOCK_STREAM);
      	soc_addr={0};
	soc_addr.sun_family=AF_UNIX;
	strcpy(soc_addr.sun_path,"/tmp/musicd.sock");

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
	while((opt=getopt_long(argc,argv,"Shp:arsnv",long_opts,&option_index))!=-1)
	{
		switch(opt)
		{
			case 'S': printf("start");break;
			case 'h': printf("shuffle");break;
			case 'p': printf("play single file");start_norm_deamon();break;
			case 'a': snd_msg_soc("0");break;
			case 'r': snd_msg_soc("2");break;
			case 's': snd_msg_soc("5");break;
			case 'n': snd_msg_soc("3");break;
			case 'v': snd_msg_soc("4");break;
			default:
				  printf("unknown option");break;
		}
	}
	close(sock);
	return 0;
}
