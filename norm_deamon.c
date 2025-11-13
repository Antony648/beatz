#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#define 	PLAY	1
#define		PAUSE 	0
#define		RESUME	2
#define 	NEXT	3
#define 	PREV	4
#define		SUICIDE 5

pthread_mutex_t lock;
int master;
struct song
{
	char*name;
	bool played;
};

void basic_play(struct song* song_list,int len)
{
	for(int i=0;i<len;i++)
		play_song(song_list[i].name);
}

bool is_music_file(char* file_name)
{
	int len=strlen(file_name);
	file_name+=(len-4);
	if(!strncmp(file_name,".wav",4))
		return true;
	return false;
}

int listfile(struct song* song_list)
{
	DIR* dirstream=opendir("~/Music");
	if(!dirstream)
		return -1;
	struct dirent* tmp;
	for(int count=0;count<100;count++)
	{
		tmp=readdir(dirstream);
		if(!tmp)
		{
			if(!is_music_file(tmp->d_name))
				continue;
			song_list->name=tmp->d_name;//song list points to a string memory that contains the name of the file
			song_list++;			 
		}
		else
			return count;
	}
	return count;	//overflow
}

void player_func()
{
	//open files in ~/Music directory....
	//check if they are .wav files
	//if so play them in normal order
	struct song song_list[100]={0};
	int len=listfile();
	if(len<0)
	{
		printf("could not open directory");
		exit(0);
	}
	if(!len)
	{
		printf("~/Music folder empty");
		exit(0)
	}
	//now song_list will contain all the paths...
	

		
}

void  interface_func()
{
	char read_buf[5]={0,0,0,0,0};int temp;
	int server_fd=socket(AF_UNIX,SOCK_STREAM,0);
	bind(server_fd,"/tmp/musicd.sock");
	listen(server_fd);
	int client_id=accept(server_id);

	while(true)
	{
		read(client_fd,read_buf,1);	//max len is 1 
	
		temp=atoi(read_buf);

		pthread_mutex_lock(&lock);
		master =temp;
		pthread_mutex_unlock(&lock);

		if(temp==SUICIDE)
			return;
		usleep(50000);
	}
	return;
}
int main()
{
	pthread_t interface,player;
	//create the /tmp/musicd.pid and /tmp/musicd.sock
	int fd=creat("/tmp/musicd.pid",0644);
	if(fd<0)
	{
		printf("failed in creating pid file");
		return 0;
	}
	char pid_val[6];
	sprintf(pid_val,"%d",get_pid());
	write(fd,pid_val,len(pid_val));

	int fd1=creat("/tmp/musicd.sock",0644);
	if(fd1<0)
	{
		printf("failed in creating socketfile");
		close(fd);
		unlink("/tmp/musicd.pid");
		return 0;
	}
	//at this point deamon has created the pid file written to it
	//also created sock file for communication....
	
	pthread_mutex_init(&lock);
	master =0;
	pthread_create(&player,NULL,player_func,NULL);
	pthread_create(&interface,NULL,interface_func,NULL);

	pthread_join(interface,NULL);
	pthread_join(player,NULL);
	//destroy the /tmp/musicd.pid and /tmp/musicd.sock
	close(fd1);
	close(fd);
	unlink("/tmp/musicd.pid");
	unlink("/tmp/musicd.sock");
	return 0;
}
