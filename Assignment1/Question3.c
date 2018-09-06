/********************************************************************************************
@authors
Vishal Chokala
Prateek Gupta
Tanmay Kulkarni
Preamble:
1. The objective of the program is to extract two numbers from a PREDEFINED set of numbers and
  then make the parent sleep for x/3 and the child for y%3 seconds. 
2. In addition, the numbers extracted should not be a repetition of the previous.
3. The execution should stop when all the numbers have been extracted.
********************************************************************************************/
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>
#include<sys/wait.h>
#include<errno.h>
#include<time.h>
#include<math.h>

int isPrime(int n){
	for(int i=2;i<=(int)sqrt(n);i++){
		if(n%i == 0) return 0;
	}
	return 1;
}

int main(){
	int arr[] = {11,12,13,14,15,16,17,18,19,20};//Initial input.
	int visited[10];
	int fd[2];
	memset(visited,0,sizeof(visited));
	int count = 0;
	pipe(fd);
	int sum=0;
	int cid = fork();
	srand(time(0));//getting seed from cpu time.
	if(cid == 0){
		int buf;
		close(fd[1]);
		while(1){
			if(read(fd[0],&buf,sizeof(int)) <= 0){sleep(1);printf("no input\n");continue;}
			printf("CHILD %d %d\n",buf,isPrime(buf));if(isPrime(buf)==1)sum+=buf;printf("the sum is:%d",sum);
			sleep(buf%3);
		}
		printf("Child deleted\n");
	}
	else{
		close(fd[0]);
		while(1){
			if(count == 10) {kill(cid, SIGKILL);exit(0);}
			int x = rand()%10;
			int y = rand()%10;
			if(visited[x] != 0 || visited[y] != 0 || x==y){
				continue;
			}
			else{
				count+=2;
				visited[x] = 1;
				visited[y] = 1;
				write(fd[1],&arr[y],sizeof(int));
				printf("values generated:%d %d\n",arr[x],arr[y]);
				printf("Parent goes to sleep for %d\n",arr[x]/3);
				sleep(arr[x]/3);
			}
		}
	}
	return 0;
}