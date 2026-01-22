#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <alsa/asoundlib.h>
#include <stdbool.h>

#define DEVICE "default"

#define SOCKET_PATH "/tmp/musicd.sock"
#define BUF_LEN 256

/* Minimal WAV header (PCM only) */
typedef struct {
    char     riff[4];        // "RIFF"
    uint32_t size;
    char     wave[4];        // "WAVE"
    char     fmt[4];         // "fmt "
    uint32_t fmt_size;       // 16 for PCM
    uint16_t audio_format;   // 1 = PCM
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char     data[4];        // "data"
    uint32_t data_size;
} wav_header_t;
enum state_t
{
	PLAYING,
	PAUSED
};
char* a[3]={"/home/anto/Music/song.wav","/home/anto/Music/song2.wav","/home/anto/Music/song3.wav"};
enum state_t 	STATE;
bool state_change;
int handle_command(int client_fd)
{
	char buffer[256];
	ssize_t bytes_read=read(client_fd,buffer,(size_t)256);

	if(bytes_read>0)
	{
		if(bytes_read>= sizeof(buffer))
			bytes_read=sizeof(buffer)-1;
		buffer[bytes_read]='\0';
		printf("message from client:%s\n",buffer);
		for(int i=0;buffer[i]!='\0';i++)
			if(buffer[i]=='\n' || buffer[i]=='\r')
				buffer[i]='\0';
		if(!strcmp("play",buffer)||!strcmp("resume",buffer))
		{
			if(STATE!=PLAYING) state_change=true;
			STATE=PLAYING;
		}
		if(!strcmp("pause",buffer))
		{
			if(STATE!=PAUSED) state_change=true;
			STATE=PAUSED;
		}
		write(client_fd,"ok\n",3);
		if(!strcmp("previous",buffer))
			return 1;
		if(!strcmp("next",buffer))
			return 2;
		
	}
	else
	{
		//errror in reading or client connection terminated
		return 3;
	}
	return 0;
}
int main(int argc, char *argv[])
{
	STATE=PAUSED;int rtn;
	state_change=false;
	int server_fd,client_fd;
	struct sockaddr_un socket_addr;
	char buffer[BUF_LEN];

	unlink(SOCKET_PATH);
	server_fd=socket(AF_UNIX,SOCK_STREAM,0); //create socket
	if(server_fd <0)
	{
		perror("error in socket creation");exit(1);
	}

	memset(&socket_addr,0,sizeof(socket_addr));
	socket_addr.sun_family=AF_UNIX;
	strncpy(socket_addr.sun_path,SOCKET_PATH,sizeof(socket_addr.sun_path));
	if(bind(server_fd,(const struct sockaddr*)&socket_addr,sizeof(socket_addr))<0)
	{
		perror("error in binding the socket");exit(1);
	}
	if(listen(server_fd,5)<0)
	{
		perror("error in listening in socket");exit(1);
	}
wait_accept:
	client_fd=accept(server_fd,NULL,NULL);
	if(client_fd<0)
	{
		goto wait_accept;
	}
	printf("connected with client_fd:%d\n",client_fd);
	int i=0;
	while(i>=0 && i<3)
	{
loop_start:
	    int fd = open(a[i], O_RDONLY);
	    if (fd < 0) {
		perror("open");
		return 1;
	    }

	    wav_header_t hdr;
	    if (read(fd, &hdr, sizeof(hdr)) != sizeof(hdr)) {
		fprintf(stderr, "Failed to read WAV header\n");
		return 1;
	    }

	    if (hdr.audio_format != 1 || hdr.bits_per_sample != 16) {
		fprintf(stderr, "Only 16-bit PCM WAV supported\n");
		return 1;
	    }

	    snd_pcm_t *pcm;
	    snd_pcm_hw_params_t *params;

	    if (snd_pcm_open(&pcm, DEVICE,SND_PCM_STREAM_PLAYBACK, 0) < 0) {
		fprintf(stderr, "ALSA open error\n");
		return 1;
	    }

	    snd_pcm_hw_params_malloc(&params);
	    snd_pcm_hw_params_any(pcm, params);

	    snd_pcm_hw_params_set_access(
		pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);

	    snd_pcm_hw_params_set_format(
		pcm, params, SND_PCM_FORMAT_S16_LE);

	    snd_pcm_hw_params_set_channels(
		pcm, params, hdr.channels);

	    snd_pcm_hw_params_set_rate(
		pcm, params, hdr.sample_rate, 0);

	    snd_pcm_hw_params(pcm, params);
	    snd_pcm_hw_params_free(params);

	    snd_pcm_prepare(pcm);

	    size_t buffer_size = 4096;
	    char buffer[4096];

	    ssize_t bytes;
		int alsa_nfds = snd_pcm_poll_descriptors_count(pcm);
		struct pollfd fds[alsa_nfds + 1];
		int nfds;
		printf("set snd_pcm settings and poll settings\n");
	    while (true)
		{
			if(STATE==PLAYING)
			{
				snd_pcm_poll_descriptors(pcm, fds, alsa_nfds);

				fds[alsa_nfds].fd=client_fd;
				fds[alsa_nfds].events=POLLIN;
				nfds=alsa_nfds+1;
			}
			else
			{
				fds[0].fd=client_fd;
				fds[0].events=POLLIN;
				nfds=1;
			}
			if(poll(fds,nfds,-1)<=0)
				continue;
			if(fds[nfds-1].revents & POLLIN)
			{
				rtn=handle_command(fds[nfds-1].fd);
				printf("state:%d",(int)STATE);
				fflush(0);
				if(rtn)
				{
					printf("drastic\n"); //remove
					if(rtn==1 )
						if( i>0)
							i-=1;
						else
							printf("aldready at first\n ");
					else if(rtn==2)
						if(i<2)
							i+=1;
						else
							printf("aldready at end\n");
					else//if(rtn ==3)
					{
						//connection terminated or error
						printf("error_restablish conncetion\n");
						snd_pcm_drain(pcm);
						snd_pcm_prepare(pcm);
						snd_pcm_close(pcm);
						close(fd);
						close(client_fd);
						rtn=0;
						STATE=PAUSED;
						goto wait_accept;
					}

					printf("song change\n");
					snd_pcm_prepare(pcm);
					snd_pcm_drain(pcm);
					snd_pcm_close(pcm);
					close(fd);
					rtn=0;
					goto loop_start;

				}

			}
			if(STATE==PAUSED && state_change)
			{
				printf("play->pause\n");
				snd_pcm_pause(pcm,1);
				state_change=false;
				continue;
			}
			unsigned short revents=0;
			if(STATE==PLAYING)
			{
				if(state_change)
				{
					printf("pause->play\n");
					snd_pcm_pause(pcm,0);
					state_change=false;

				}
				snd_pcm_poll_descriptors_revents(pcm, fds,alsa_nfds,&revents);
			}

			if(STATE==PLAYING && (revents &POLLOUT))
			{
				//printf("playing\n");
				
				if((bytes = read(fd, buffer, buffer_size)) <= 0)
					break;
				snd_pcm_sframes_t frames =
					bytes / (hdr.channels * 2);

				snd_pcm_sframes_t ret =
					snd_pcm_writei(pcm, buffer, frames);

				if (ret == -EPIPE)
				{
					snd_pcm_prepare(pcm);
				}
				else if (ret < 0)
				{
					fprintf(stderr, "ALSA write error: %s\n",
						snd_strerror(ret));
					break;
				}
			}
	    }

	    snd_pcm_drain(pcm);
	    snd_pcm_close(pcm);
		close(fd);
		i++;
	}

	return 0;
}

