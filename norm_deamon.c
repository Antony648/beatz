#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <alsa/asoundlib.h>
#define 	PLAY	1
#define		PAUSE 	6
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

void play_single(song song1,int* i_addr)
{
	char path[20]="~/Music/";
	strcat(path,song1.name);
	snd_pcm_t* handle;
	snd_pcm_hw_params_t* params;
	SF_INFO info;
	SNDFILE *file;
	unsigned int buffer_size,period_size;
	int frame_size,user_buffer_bytes;
	short * user_buffer;

	//open a handler
	snd_pcm_open(&handler,"default",SND_PCM_STREAM_PLAYBACK,0);
	snd_pcm_hw_params_alloca(&params);
	snd_pcm_hw_params_any(handler,params);

	snd_pcm_hw_params_set_access(handle,params,SND_PCM_ACCESS_RW_INTERLEAVED);

	file=sf_open(path,SFM_READ,&info);

	snd_pcm_hw_params_set_rate(handler, params,info.samplerate);
	snd_pcm_hw_params_set_channels(handler,params,info.channels);
	// need to conver sf format to snd_pcm format
	int format=info.format && SF_FORMAT_SUBMASK;
	snd_pcm_format_t alsa_format;
	switch(format)
	{
		case SF_FORMAT_PCM_16:
			asla_format=SND_PCM_FORMAT_S16_LE;
			break;
		case SF_FORMAT_PCM_24:
			asla_format=SND_PCM_FORMAT_S24_LE;
			break;
		case SF_FORMAT_PCM_32:
			asla_format=SND_PCM_FORMAT_S32_LE;
			break;
		case SF_FORMAT_PCM_FLOAT:
			asla_format=SND_PCM_FORMAT_FLOAT_LE;
			break;
		default:
			printf("unsupported format");
			return;	//will return function and play next song
	}
	snd_pcm_hw_params_set_format(handle,params,alsa_format);
	buffer_size=4096;
	
	snd_pcm_hw_params_set_buffer_size_near(handler,params,&buffer_size);	//setting near values

	period_size=buffer_size/4;
	snd_pcm_hw_params_set_period_size_near(handler,params,&period_size): //setting near values

	snd_pcm_hw_params(handle,params);//after settng near values
					 //asking alsa for set value
	
	snd_pcm_hw_params_set_buffer_size(handler,params,&buffer_size);
	snd_pcm_hw_params_set_period_size(handler,params,&period_size,0);
	frame_size=info.channels * sizeof(short);
	user_buffer_bytes= period_size * frame_size;
	
	user_buffer=(short*)malloc(user_buffer_bytes);
	//loop with critical section...
	
	free((void*) user_buffer);


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
	//now song_list will contain all the names...
	for(int i=0;i<len;i++)
		play_single(song_list[i],&i);
	

		
}

void  interface_func()
{
	char read_buf[5]={0,0,0,0,0};int tmp,val;

	int server_fd=socket(AF_UNIX,SOCK_STREAM,0);
	struct sockaddr_un sock_addr={0};

	sock_addr.sun_family=AF_UNIX;
	strcpy(sock_addr.sun_path,"/tmp/musicd.sock");
	
	unlink(sock_addr.sun_path);
	bind(server_fd,&sock_addr,sizeof(sock_addr));
	listen(server_fd,1);

	int client_fd=accept(server_fd,NULL,NULL);
	struct pollfd poll_fd;
	poll_fd.fd=client_fd;
	poll_fd.events=POLLIN;

	while(true)
	{
		val=poll(&poll_fd,1,-1);
		if(val >0 && (poll_fd.revents & POLLIN))
		{
			if(read(client_fd,read_buf,1))
			{
				tmp=atoi(read_buf);
				pthread_mutex_lock(&lock);
				master=tmp;
				pthread_mutex_unlock(&lock);

				if(tmp==SUICIDE)
					break;
			}
			
		}
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
	write(fd,pid_val,strlen(pid_val));

	
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
