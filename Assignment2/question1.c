#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<errno.h>
#include"header.h"
#define MAX 100
#define NEW_PROCESS 0
#define CPU_BURST_COMPLETE 1
#define TIMER_EXPIRED 2
#define CANCELED -1
#define BUFF_SIZE 10
#define FCFS "FCFS"
#define MULTQ "MULTQ"
#define FCFS_CODE 0
#define MULTQ_CODE 1
#define TIMEQUANTA 4
#define BURST_LIMIT 8

typedef struct Event{
	int type;
	int time;
	int pid;
}event;

typedef struct Process{
	int pid;
	int cpuTime;
	int waitTime;
	int arrivalTime;
	int state;
}process;

typedef struct Node{
	process prcs;
	struct Node* next;
}node;

process processTable[PROCESS_COUNT];
int cpu  =  -1;
int schedMode;

void initSchedMode(){
	if(strcmp(SCHEDULING_MODE,FCFS) == 0){
		schedMode = FCFS_CODE;
	}
	else if(strcmp(SCHEDULING_MODE,MULTQ) == 0){
		schedMode = MULTQ_CODE;
	}
}

void initProcess(process* tempProcess,int count,int cpuTime,int arrivalTime){
	tempProcess->pid = count;
	tempProcess->cpuTime = cpuTime;
	tempProcess-> arrivalTime = arrivalTime;
	tempProcess->waitTime = 0;
}

void initEvent(event* tempEvent,int type,int time,int pid){
	tempEvent->type = type;
	tempEvent->time = time;
	tempEvent->pid = pid;
}

void swap(event* a,event* b){
	event temp = *a;
	*a = *b;
	*b = temp;
}

void insert(event incomingEvent,event* eventHeap,int* numEvents){
	eventHeap[*numEvents] = incomingEvent;
	*numEvents = *numEvents+1;
	int currEvent = *numEvents-1;
	int parent = (currEvent-1)/2;

	while(currEvent>0 && eventHeap[currEvent].time<eventHeap[parent].time){
		swap(&eventHeap[currEvent],&eventHeap[parent]);
		currEvent = parent;
		parent = (parent-1)/2;
	}
}

event delete(event* eventHeap,int* numEvents){
	event returnVal = eventHeap[0];
	eventHeap[0] = eventHeap[*numEvents-1];
	*numEvents -= 1;
	int currEvent = 0;
	int child1 = 2*currEvent+1;
	int child2 = 2*currEvent+2;

	while(child2 <= *numEvents){
		int smallerChild = eventHeap[child1].time<eventHeap[child2].time?child1:child2;
		if(eventHeap[currEvent].time>eventHeap[smallerChild].time){
			swap(&eventHeap[currEvent],&eventHeap[smallerChild]);
			currEvent = smallerChild;
		}
		else{
			break;
		}

		child1 = 2*currEvent+1;
		child2 = 2*currEvent+2;
	}

	return returnVal;
}

void getData(event* eventHeap,int* numEvents){
	FILE* fp = NULL;
	int count = 0;
	fp = fopen(FILE_NAME,"r");

	if(fp == NULL){
		perror("File not found");
		exit(1);
	}

	char buffer[BUFF_SIZE];
	while(fgets(buffer,BUFF_SIZE,fp) != NULL){
		process* tempProcess = (process*)calloc(1,sizeof(process));
		event* tempEvent = (event*)calloc(1,sizeof(event));
		int arrivalTime,cpuTime;
		sscanf(buffer,"%d %d",&arrivalTime,&cpuTime);

		initProcess(tempProcess,count,cpuTime,arrivalTime);
		initEvent(tempEvent,NEW_PROCESS,arrivalTime,count);

		insert(*tempEvent,eventHeap,numEvents);
		processTable[count] = *tempProcess;

		count++;
	}
}

void eventHeapToString(event* eventHeap){
	for(int i = 0;i<PROCESS_COUNT;i++){
		printf("type:%d time:%d pid:%d\n",eventHeap[i].type,eventHeap[i].time,eventHeap[i].pid);
	}
}

void insertToQueue(node** pointerToHead,node** pointerToTail,process newProcess){
	node* temp = (node*)calloc(1,sizeof(node));
	temp->prcs = newProcess;
	temp->next = NULL;

	if(*pointerToHead == NULL){

		*pointerToHead = temp;
		*pointerToTail = *pointerToHead;
	}
	else{
		(*pointerToTail)->next = temp;
		*pointerToTail = (*pointerToTail)->next;
	}
}
void addToStart(node** pointerToHead,node** pointerToTail,process newProcess){
	node* temp=(node*)calloc(1,sizeof(node));
	temp->prcs=newProcess;
	temp->next=NULL;
	if(*pointerToHead==NULL){	
		*pointerToHead=temp;
		*pointerToTail=*pointerToHead;
	}
	else{
		temp->next=*pointerToHead;
		*pointerToHead=temp;
	}
}

node* deleteFromQueue(node** pointerToHead,node** pointerToTail){
	node* returnVal;

	if(*pointerToHead == NULL){
		return NULL;
	}
	else if((*pointerToHead)->next == NULL){
		returnVal = *pointerToHead;
		*pointerToHead = NULL;
		*pointerToTail = NULL;

		return returnVal;
	}
	else{
		returnVal = *pointerToHead;
		*pointerToHead = (*pointerToHead)->next;

		return returnVal;
	}
}

event* createNewEvent(process newProcess,int time){
	event* newEvent = (event*)calloc(1,sizeof(event));
	newEvent->pid = newProcess.pid;

	if(schedMode == FCFS_CODE){
		newEvent->type = CPU_BURST_COMPLETE;
		newEvent->time = time+newProcess.cpuTime;
	}
	else if(schedMode == MULTQ_CODE){
		if(newProcess.cpuTime <= BURST_LIMIT){
			if(newProcess.cpuTime>TIMEQUANTA){
				newEvent->type = TIMER_EXPIRED;
				newEvent->time = time+TIMEQUANTA;
				processTable[newProcess.pid].cpuTime -= TIMEQUANTA;
			}
			else{
				newEvent->type = CPU_BURST_COMPLETE;
				newEvent->time = time+newProcess.cpuTime;
			}
		}
		else{
			newEvent->type = CPU_BURST_COMPLETE;
			newEvent->time = time+newProcess.cpuTime;
		}
	}

	return newEvent;
}

void dispatcher(node** pointerToHead1,node** pointerToTail1,node** pointerToHead2,node** pointerToTail2,int* numEvents,event* eventHeap,int pid,int time){

	if(schedMode == FCFS_CODE){
		if(cpu == -1){
			node* ptrProcess = deleteFromQueue(pointerToHead1,pointerToTail1);
			if(ptrProcess == NULL){
				printf("\nNo more processes to run....\n");
				return ;
			}
			else{
				// Run the process
				printf("TIME:%d The process:%d has commenced execution and will take %ds\n",time,(ptrProcess->prcs).pid,processTable[(ptrProcess->prcs).pid].cpuTime);
				cpu = (ptrProcess->prcs).pid;
				processTable[cpu].waitTime+=time-processTable[cpu].arrivalTime; // updated the wait time as we got the cpu
			}
			event* newEvent = createNewEvent(ptrProcess->prcs,time); //Create event according to type, accounted by the global vars

			insert(*newEvent,eventHeap,numEvents);
			free(ptrProcess);
		}
	}
	else{ // for MULTQ
		if(cpu == -1){
			node* ptrProcess = deleteFromQueue(pointerToHead1,pointerToTail1);
			if(ptrProcess == NULL){
				ptrProcess = deleteFromQueue(pointerToHead2,pointerToTail2);
				if(ptrProcess == NULL){
					printf("\nNo more processes to run....\n");
					return ;
				}
				else{
					printf("TIME:%d The process:%d has commenced execution and will take %ds\n",time,(ptrProcess->prcs).pid,processTable[(ptrProcess->prcs).pid].cpuTime);
					cpu = (ptrProcess->prcs).pid;
					processTable[cpu].waitTime+=time-processTable[cpu].arrivalTime; // updated the wait time as we got cpu
					processTable[cpu].arrivalTime=time; // so as to account for the runtime in case it gets preempted
				}
			}
			else{
				printf("TIME:%d The process:%d has commenced execution and will take %ds\n",time,(ptrProcess->prcs).pid,processTable[(ptrProcess->prcs).pid].cpuTime);
				cpu = (ptrProcess->prcs).pid;
				processTable[cpu].waitTime+=time-processTable[cpu].arrivalTime; // update the wait time as we get the cpu
			}

			event* newEvent = createNewEvent(ptrProcess->prcs,time); //Create event according to type, accounted by the global vars
			insert(*newEvent,eventHeap,numEvents);
			free(ptrProcess);
		}
		else{
			process runningProcess=processTable[cpu];
			if(runningProcess.cpuTime>BURST_LIMIT && processTable[pid].cpuTime<=BURST_LIMIT){ // preempting the running
				for(int i=0;i<*numEvents;i++){
					if(eventHeap[i].pid==cpu){ //find the cpu completion event
						eventHeap[i].type=CANCELED; // cancel the event
						break;
					}
				}
				printf("TIME:%d The process:%d has been prempted\n",time,cpu);
				processTable[cpu].cpuTime-=time-processTable[cpu].arrivalTime;//This is only in the case of fcfs processes as we update when we schedule them
				processTable[cpu].arrivalTime=time; // so as to update the arrival time to the time of interrupt
				addToStart(pointerToHead2,pointerToTail2,processTable[cpu]);
				cpu=-1;
				dispatcher(pointerToHead1,pointerToTail1,pointerToHead2,pointerToTail2,numEvents,eventHeap,pid,time); // we run the dispatcher to run the new process
			}
		}
	}
}

void addProcess(int pid,node** pointerToHead1,node** pointerToTail1,node** pointerToHead2,node** pointerToTail2,int time,event* eventHeap,int* numEvents){

	printf("TIME:%d Adding the process %d from the eventheap to readyQueue\n",time,pid);
	process newProcess = processTable[pid];

	event* newEvent = NULL;
	if(schedMode == FCFS_CODE){
		insertToQueue(pointerToHead1,pointerToTail1,newProcess);
		dispatcher(pointerToHead1,pointerToTail1,NULL,NULL,numEvents,eventHeap,pid,time);
	}
	else if(schedMode == MULTQ_CODE){
		if(newProcess.cpuTime <= BURST_LIMIT){
			insertToQueue(pointerToHead1,pointerToTail1,newProcess);
			dispatcher(pointerToHead1,pointerToTail1,pointerToHead2,pointerToTail2,numEvents,eventHeap,pid,time);
		}
		else{
			insertToQueue(pointerToHead2,pointerToTail2,newProcess);
			dispatcher(pointerToHead1,pointerToTail1,pointerToHead2,pointerToTail2,numEvents,eventHeap,pid,time);
		}
	}
	else{
		perror("It seems you have tried to run a functionality that the developers are yet to implement\n");
		exit(2);
	}
}

void printReadyQueue(node* head){
	printf("The current ready queue contains:\n");
	node* temp = head;

	while(temp != NULL){
		printf("Pid:%d ArrivalTime:%d BurstDuration:%d\n",(temp->prcs).pid,(temp->prcs).arrivalTime,(temp->prcs).cpuTime);
		temp = temp->next;
	}
	printf("\n\n");
}

void processCompletion(int pid,node** pointerToHead1,node** pointerToTail1,node** pointerToHead2,node** pointerToTail2,int time,event* eventHeap,int* numEvents){
	printf("TIME:%d Process:%d arrived at %d has completed execution\n\n",time,pid,processTable[pid].arrivalTime);

	if(schedMode == MULTQ_CODE){
		printf("ROUNDROBIN\n");
		printReadyQueue(*pointerToHead1);
		printf("FCFS\n");
		printReadyQueue(*pointerToHead2);
	}
	else{
		printf("FCFS\n");
		printReadyQueue(*pointerToHead1);
	}

	cpu = -1;
	if(schedMode == FCFS_CODE){
		dispatcher(pointerToHead1,pointerToTail1,NULL,NULL,numEvents,eventHeap,pid,time);
	}
	else if(schedMode == MULTQ_CODE){
		dispatcher(pointerToHead1,pointerToTail1,pointerToHead2,pointerToTail2,numEvents,eventHeap,pid,time);
	}
}

void bringToBack(int pid,node** pointerToHead1,node** pointerToTail1,node** pointerToHead2,node** pointerToTail2,int time,event* eventHeap,int* numEvents){

	printf("TIME:%d Process:%d has timed out\n",time,pid);
	cpu = -1;

	addProcess(pid,pointerToHead1,pointerToTail1,pointerToHead2,pointerToTail2,time,eventHeap,numEvents);
	dispatcher(pointerToHead1,pointerToTail1,pointerToHead2,pointerToTail2,numEvents,eventHeap,pid,time);
	processTable[pid].arrivalTime = time;
}

void averageWaitTime(){
	double value=0;
	for(int i=0;i<PROCESS_COUNT;i++){
		value+=processTable[i].waitTime;
	}
	if(PROCESS_COUNT==0){
		printf("No Processes present\n");
	}
	else{
		value/=PROCESS_COUNT;
		printf("\n\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		printf("The Average wait time is\n");
		printf("%lf\n",value);
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	}
}
int main(){
	event eventHeap[MAX];
	int numEvents = 0;
	node* headOfReadyQueue = NULL;
	node* tailOfReadyQueue = NULL;

	node* headOfTopQ = NULL;
	node* tailOfTopQ = NULL;
	node* headOfBottomQ = NULL;
	node* tailOfBottomQ = NULL;

	memset(eventHeap,0,sizeof(eventHeap));
	initSchedMode();

	getData(eventHeap,&numEvents);

	int time = 0;


	while(numEvents>0){
		event temp = delete(eventHeap,&numEvents);
		time = temp.time;
		// printf("TIME:%d The event of type %d regarding pid:%d has arrived\n\n",time,temp.type,temp.pid);
		switch(temp.type){
			case NEW_PROCESS:{
				if(schedMode == FCFS_CODE){
					addProcess(temp.pid,&headOfReadyQueue,&tailOfReadyQueue,NULL,NULL,time,eventHeap,&numEvents);
					break;
				}
				else if(schedMode == MULTQ_CODE){
					addProcess(temp.pid,&headOfTopQ,&tailOfTopQ,&headOfBottomQ,&tailOfBottomQ,time,eventHeap,&numEvents);
					break;
				}
			}
			case CPU_BURST_COMPLETE:{
				if(schedMode == FCFS_CODE){
					processCompletion(temp.pid,&headOfReadyQueue,&tailOfReadyQueue,NULL,NULL,time,eventHeap,&numEvents);
					break;
				}
				else if(schedMode == MULTQ_CODE){
					processCompletion(temp.pid,&headOfTopQ,&tailOfTopQ,&headOfBottomQ,&tailOfBottomQ,time,eventHeap,&numEvents);
					break;
				}
			}
			case TIMER_EXPIRED:{
				bringToBack(temp.pid,&headOfTopQ,&tailOfTopQ,&headOfBottomQ,&tailOfBottomQ,time,eventHeap,&numEvents);
				break;
			}
			default: printf("!!!!WARNING:Obtained a deleted element!!!!!\n");
		}
	}

	averageWaitTime();
	return 0;
}
