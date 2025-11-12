#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sndfile.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
struct song
{
	char*name;
	bool played;
};
int play_song(char* song_name)
{
	snd_pcm_t* handler;
	snd_pcm_params_t* params;

}
int get_next_unplayed(struct song* song_list,int random,int len)
{	
	int save_state=random;
	random++;
	while(song_list[random].played && random!=save_state)
	{
		if(random>=len)
			random=-1;
		random++;
	}
	return random;
}
void basic_rand_play(struct song* song_list,int len)
{
	int random;
	for(int i=0;i<len;i++)
	{
		random=random()%len;
		if(song_list[random].played)
			play_song(song_list[get_next_unplayed(song_list,random,len)].name);
		else
			play_song(song_list[random].name);
	}
}

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

void print_usage()
{
	printf("beatz [command]\n");
	printf("commands:\n");
	printf("--start		:start the player to play music form ~/Music\n");
	printf("--shuffle	:just like start but plays music in random order\n");
	printf("--previous	:play previous song\n");
	printf("--next		:play next song\n");
	printf("--pause		:pause current song\n");
	printf("--resume	:resume current song\n");
	printf("--stop		:stop the player\n"):
}
int main(int argc, char* argv[])
{
	//play list is assumed to be in the ~/Music directory
	//play songs one after the other or in shuffle
	//scan ~/Music and create an array of struct song*
	//keep in track of its length.....
	//if shuffle then iterate till length randomly
	//else in normal fashion send the song object 
	
	if(argc ==1)
	{
		print_usage();
		exit(0);
	}

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
	basic_play(song_list,len);
		
}
