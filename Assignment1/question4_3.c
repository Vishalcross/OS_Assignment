/********************************************************************************************
@authors
Vishal Chokala
Prateek Gupta
Tanmay Kulkarni
Preamble:
The objective of this program is to create an application that is similar to a process 
manager. It takes the number of times it show processes and how many processes at a time.
Furhtermore, it gives the option of killing processes bases on user demand.
WARNING: This program makes use of SIGKILL thus, it does not partake in clean  up. In general,
it is recommended to use SIGTERM.
NOTE: This program makes use of signals to communicate between the parent and the child. So, that 
they wake up each other with the help of a SIGHUP.
********************************************************************************************/
#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<stdlib.h>
#include<sys/signal.h>
#include<sys/wait.h>
#include<string.h>

void handler1(int sigNum){
	system("echo Parent Interrupted successfully");
}
void handler2(int sigNum){
	system("echo Child Interrupted successfully");
}
int getValue(char* c){
    int length=strlen(c);
    int val=0;
    for(int i=0;i<length;i++){
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
void forkIt(int *val){
	int fd[2];
	pipe(fd);
	int pid2=fork();
	if(pid2==0){
		int pid3=fork();
		if(pid3==0){
			dup2(fd[1],1);
			close(fd[0]);
			char loc[]="ps";
			char cmd[]="ps";
			char option[]="-eo";
			char opvals[]="pid,ppid,cmd,%cpu,%mem";
			char sort[]="--sort=-%cpu";
			execlp(loc,cmd,option,opvals,sort,NULL);
		}
		else{
			int status;
			wait(&status);
			dup2(fd[0],0);
			close(fd[1]);
			char loc[]="head";
			char cmd[]="head";
			char option[]="-n";
			char temp[10];
			sprintf(temp,"%d",(val[2]+1));
			char* number=temp;
			execlp(loc,cmd,option,number,NULL);
		}
	}
	else{
		int status;
		wait(&status);
	}
}
int main(int argc,char* argv[]){
	
	if(argc!=4){
		system("echo We dont do that here.");
		system("echo USEAGE: timeDelay sleep number_of_processes how_many_times");
		return -1;
	}
	
	int val[3];
    for(int i=1;i<argc;i++){
        val[i-1] = getValue(argv[i]);
    }
	int fd[2];
	signal(SIGHUP,handler1);
	pipe(fd);
	
	while(1){
		int pid=fork();
		if(pid==0){
			signal(SIGHUP,handler2);
			for(int i=0;i<val[1];i++){
				forkIt(val);
				sleep(val[0]);
			}
			kill(getppid(),SIGHUP);
			sleep(1000);
			int kid;
			close(fd[1]);
			read(fd[0],&kid,sizeof(kid));
			printf("The kill id:%d\n",kid);
			if(kid>0){
				int id=0;
				id=kill(kid,SIGKILL);
				if(id<0){
					printf("Termination failed\n");
				}
				else{
					printf("Successfull\n");
				}
			}
			return 0;
		}
		else{
			sleep(1000);
			printf("Enter the process to be killed\n");
			int temp;
			scanf("%d",&temp);
			close(fd[0]);
			write(fd[1],&temp,sizeof(temp));
			kill(pid,SIGHUP);
		}
	} 
}
