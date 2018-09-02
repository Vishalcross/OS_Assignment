#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>

int main(int argc, char* argv[]){
	if(argc != 3){
		system("echo We dont do that here.");
		system("echo USEAGE: newgrep regex filename");
	}
	else{
		int pid2=fork();
		if(pid2==0){
			char* regex=argv[1];
			char* filePath=argv[2];
			int fd[2];
			if(pipe(fd)==-1){
				system("echo pipe creation failed");
				return -1;
			}
			int pid=fork();
			if(pid<0){
				system("echo fork failed");
				return -1;
			}
			if(pid==0){
				char* command="grep";
				char* option="-n";
				dup2(fd[1],1);
				execlp("grep",command,option,regex,filePath,NULL);
			}
			else{
				int status;
				wait(&status);
				dup2(fd[0],0);
				char* command="cut";
				char* option2="-f1";
				char* option1="-d:";
				close(fd[1]);
				execlp("cut",command,option1,option2,NULL);
			}	
		}
		else{
			int status;
			wait(&status);
			system("echo Exection was successful");
		}
	}
	return 0;
}
