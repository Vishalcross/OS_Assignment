#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/signal.h>
#include"ferk.h"
void handler(int s){
	printf("You got your arse handled bitch! %d\n",s);
}
int main(){
	int x=ferk();
	signal(SIGCHLD,handler);
	signal(SIGINT,handler);
	signal(SIGTERM,handler);
	signal(SIGKILL,handler);
	if(x==0){
		sleep(1);
		printf("Ferked in the arse\n");
	}
	else if(x>0){
		sleep(2);
		printf("What the ferk %d\n",x);
	}
	else{
		printf("ferk you os\n");
	}
	return 0;
}
