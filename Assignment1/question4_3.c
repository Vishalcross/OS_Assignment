/********************************************************************************************
@authors
Vishal Chokala
Prateek Gupta
Tanmay Kulkarni
Preamble:
The objective of this program is to create an application that is similar to a process 
manager. It takes the number of times it show processes and how many processes at a time.
Furhtermore, it gives the option of killing processes bases on user demand.
********************************************************************************************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<errno.h>

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
	
	int fd1[2];
	int fd2[2];
	pipe(fd1);
	pipe(fd2);
	
	int pid=fork();
	if(pid==0){
		while(1){
			for(int i=0;i<val[2];i++){
				forkIt(val);
				sleep(val[0]);
			}
			close(fd1[0]);
			int success=200;
			write(fd1[1],&success,sizeof(int));
			int val=-2123;
			close(fd2[1]);
			read(fd2[0],&val,sizeof(int));
			if(val>0){
				int id;
				id=kill(val,SIGTERM);
				if(id>=0){
					printf("Termination Successful\n");
				}
				else{
					printf("Termination failed\n");
				}
			}
			else if(val==-2123){
				printf("Value did not change\n");
			}
		}
	}
	else{
		while(1){
			int val=-1;
			close(fd1[1]);
			int readVal;
			readVal=read(fd1[0],&val,sizeof(int));
			printf("read returned:%d\n",readVal);
			if(val!=-1){
				int kid;
				printf("Enter the pid of the process to be killed\n");
				scanf("%d",&kid);
				close(fd2[0]);
				write(fd2[1],&kid,sizeof(int));
			}
			else if(readVal==-1){
				switch(errno){
					case EBADF:printf("fd is not a valid file descriptor or is not open for reading.\n");break;
					case EFAULT: printf("buf is outside your accessible address space.\n");break;
					case EINTR:  printf("The call was interrupted by a signal before any data was read; see signal(7).\n");break;
					case EINVAL: printf("fd is attached to an object which is unsuitable for reading; or the file was opened with the O_DIRECT flag, and either the address specified in  buf,\n");
								 printf("the value specified in count, or the current file offset is not suitably aligned\n");
								 printf("fd was created via a call to timerfd_create(2) and the wrong size buffer was given to read(); see timerfd_create(2) for further information.\n");break;
					case EIO: 	 printf("I/O  error.   This will happen for example when the process is in a background process group, tries to read from its controlling terminal, and either\n");
								 printf("it is ignoring or blocking SIGTTIN or its process group is orphaned.  It may also occur when there is a low-level I/O error while reading from a disk\n");
								 printf("or tape.\n");break;
				}
			}
		}
	}
}
