#include<stdlib.h>
#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char* argv[]){
	if(argc != 3){
		system("echo We dont do that here.");
		system("echo USEAGE: newgrep regex filename");
	}
	else{
		char* regex = argv[1];
		char* filePath = argv[2];
		printf("regex:%s filePath:%s\n",regex,filePath);
		FILE *fp=NULL;
		int fd[2];
		if(pipe(fd)==-1){
			system("echo pipe failed");
			return -1;
		}
		int pid=fork();
		if(pid<0){
			system("echo The fork failed");
			return -1;
		}
		if(pid==0){
			printf("Entered child\n");
			char start[]="/bin/grep -n ";
			strcat(start,regex);
			strcat(start," ");
			strcat(start,filePath);
			printf("%s\n",start);
			fp=popen(start,"r");
			printf("%p\n",fp);
			char temp[100];
			char* answer=(char*)calloc(sizeof(char),1000);
			while(fgets(temp,sizeof(temp),fp)!=NULL){
				strcat(answer,temp);
			}
			printf("%s",answer);
			write(fd[1],answer,strlen(answer));
		}
		else{
			int pr,status;
			pr=wait(&status);
			char answer[1000];
			read(fd[0],answer,(100));
			printf("%s",answer);
			/*char temp[100];
			while(fgets(temp,sizeof(temp),fp)!=NULL){
				printf("%s",temp);
			}*/
		}
	}
	return 0;
}
