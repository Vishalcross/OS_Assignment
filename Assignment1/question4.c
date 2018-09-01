#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>
#include<stdio.h>

int getValue(char* c){
	int length=strlen(c);
	int val=0;
	for(int i=length-1;i>=0;i--){
		val*=10;
		int temp=c[i]-'0';
		if(temp<0 || temp>10){
			system("echo the input seems to be incorrect.");
			system("echo USEAGE: timeDelay sleep number_of_processes how_many_times");
			return -1;
		}
		val+=temp;
	}
	return val;
}
int main(int argc,char* argv[]){
	if(argc!=4){
		system("echo We dont do that here.");
		system("echo USEAGE: timeDelay sleep number_of_processes how_many_times");
		return -1;
	}
	int vars[3];
	for(int i=0;i<3;i++){
		int temp=(getValue(argv[i+1]));
		if(temp==-1){
			return -1;
		}
		if(temp<=0){
			system("echo input has to be positive.");
			return -1;
		}
		vars[i]=temp;
	}
	int fd[2];
	int fd1[2];
	if(pipe(fd)==-1){
		system("echo Pipe creation failed");
		return -1;
	}
	if(pipe(fd1)==-1){
		system("echo Pipe creation failed");
		return -1;
	}
	while(1){
		for(int i=0;i<vars[2];i++){
			int pid1=fork();
			if(pid1==0){
				int pid2=fork();
				if(pid2==0){
					char start[]="ps";
					char option[]="-eo";
					char desc[]="pid,ppid,cmd,%mem,%cpu";
					char addn[]="--sort=-%cpu";
					printf("%s\n",start);
					close(fd[0]);
					dup2(fd[1],1);
					execlp("ps",start,option,desc,addn,NULL);
				}
				else{
					int status;
					wait(&status);
					char temp[20];
					sprintf(temp,"%d",vars[1]+1);
					char start[]="head";
					char option[]="-n";
					dup2(fd[0],0);
					close(fd[1]);
					printf("%s\n",start);
					execlp("head",start,option,temp,NULL);
				}
			}
			int status;
			wait(&status);
			int kid=2;
			if(i+1==vars[2]){
				printf("What do you wanna kill today?\n");
				scanf("%d",&kid);
				if(kid==-1){
					return 0;
				}
				else{					
					write(fd1[1],&kid,sizeof(kid));
					int pid3=fork();
					if(pid3==0){
						int kid;
						if(read(fd1[0],&kid,sizeof(kid)>0)){
							if(kill(kid,SIGINT)==0){
								system("echo Termination was successful");
							}
							else{
								system("echo Termination failed");
							}
						}
					}
					wait(&status);
				}
			}
			sleep(vars[0]);
		}
	}
	return 0;
}