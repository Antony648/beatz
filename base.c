#include <stdio.h>
#include <alsa/asoundlib.h>
#include <stdint.h>
#include <sndfile.h>
#define STEREO 2
#define MONO 1
int play_music(char* file_path,snd_pcm_t* handler,snd_pcm_hw_params_t* params)
{
	sndfile* music_f;
	SF_INFO sfinfo;
	music_f=sf_open(file_path,SFM_READ,&sfinfo);
	if(!music_f)
	{
		printf("failed in opening file\n");
		return -1;
	}
	snd_pcm_hw_params_channels(handler,params,sfinfo.channels);
//	snd_pcm_hw_params_set_rate(handler,params,sfinfo.samplerate,0);
	
	snd_pcm
	return 0;
}
int main()
{
	snd_pcm_t* handler;
	snd_pcm_hw_params_t* params;
	int val;uint32_t rate;


	val=snd_pcm_open(&handler,"default",SND_PCM_STREAM_PLAYBACK,0);
	if(val<0)
	{
		printf("error in opening file");
		return -1;
	}
	snd_pcm_hw_params_alloca(&params);
	snd_pcm_hw_params_any(handle,params);
	//init access
	//format
	//channel
	//sample rate
	snd_pcm_hw_params_set_access(handle,params,SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(handle,params,SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channel(handle,params,STEREO);
	rate=41400;
	snd_pcm_hw_params_set_rate_near(handle,params,&rate,0); //any direction so 0
	//now imprint the init params object to handler
	val=snd_pcm_hw_params(handler,params);
	if(val<0)
	{
		printf("failed to set the snd_pcm_params_t object");
		snd_pcm_close(handler);
		return -1;
	}
	printf("pcm handler name:%s\n",snd_pcm_name(handler));

	//now open the wav file...
	music_f= sf_open("~/Music/music.wav",SFM_READ,&sfinfo);
	if(!music_f)
	{
		printf("failure in opening music file");
		snd_pcm_close(handler);
		return -1;
	}
	//code to read sfifo and set params for read and buffer 
	if(!play_music("~/Music/sound.wav",handler,params))
		return -1;
	//size properly... malloc and free



	sf_close(music_f);
	snd_pcm_drain(handler);
	snd_pcm_close(handler);
	
	return 0;
}
