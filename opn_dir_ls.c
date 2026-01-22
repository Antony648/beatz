#include <stdlib.h>
#include <stdio.h>
#include <sys/un.h>
#include <dirent.h>
#include <string.h>
struct table_ent
{
	char* file_name;
	struct table_ent* next;
	struct table_ent* prev;
};
int main()
{
	DIR* target_dir=opendir("/home/anto/Music");
	if(target_dir==NULL)
	{
		perror("failed in opening directory");return 1;
	}
	struct dirent *target_dirent;
	struct table_ent *start=NULL,*current;
	char* buffer;int len;
	for(target_dirent=readdir(target_dir);target_dirent;target_dirent=readdir(target_dir))
	{	
		if(target_dirent->d_type != DT_REG)
			continue;
		len=strlen(target_dirent->d_name);
		buffer=(char*)malloc(len+1);
		strncpy(buffer,target_dirent->d_name,len);
		buffer[len]='\0';
		if(strcmp(&buffer[len-4],".wav"))
		{
			free(buffer);
			continue;
		}
		if(!start)
		{
			start=malloc(sizeof(start));
			start->prev=NULL;
			current=start;
		}
		else
			if(current)
			{
				current->next=malloc(sizeof(current));
				current->next->prev=current;
				current=current->next;
			}
		current->file_name=buffer;
		current->next=NULL;
		
	}
	start->prev=current;
	current->next=start;
	//print and destroy
	struct table_ent* k=start;
	while(start->next !=k)
	{
		printf("%s\n",start->file_name);
		free(start->file_name);
		current=start;
		start=start->next;
		free(current);
	}
	printf("%s\n",start->file_name);
	free(start->file_name);
	free(start);

	return 0;

}
