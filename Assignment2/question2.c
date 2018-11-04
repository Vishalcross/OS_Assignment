/******************************************************************************************************************************************
 *
 * #ToDo finish resource leak
 *
 ******************************************************************************************************************************************/
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/types.h>
#include<errno.h>
#include<semaphore.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<math.h>

#define NUMBEROFTYPES 4
#define NUMBEROFTHREADS 8
#define PERCENT 100
#define ACQUIRETIME 1

sem_t* countingSemaphores[NUMBEROFTYPES];
int inUse[NUMBEROFTYPES];
sem_t* binSemaphore;
sem_t* binSemaphore2;
int status[NUMBEROFTHREADS];
int requirement[NUMBEROFTHREADS];


typedef struct ArgumentList{
	int threadNum;
	int* resourceValues;
	int* timesExecuted;
	int** properties;
	int* sizes;
	double* retreivalValues;
}arglist;


int* init(int *numberOfTimes, int* timesExecuted, double* retreivalValues){
	// Define the number of times each process should run and the initial values for each resource.
	printf("Obtaining input from the user...\n\n");
	int* resourceValues = (int*)calloc(NUMBEROFTYPES, sizeof(int));
	if(resourceValues == NULL){
		perror("Unable to initialize resource values\n");
		exit(1);
	}
	printf("enter the number of times each process should run\n");
	scanf("%d", numberOfTimes);
	printf("Enter the inital resource values of each\n");
	for(int i = 0;i<NUMBEROFTYPES;i++){
		scanf("%d", &resourceValues[i]);
	}
	for(int i = 0;i<NUMBEROFTHREADS;i++){
		timesExecuted[i] = *numberOfTimes;
	}
	printf("Enter the probability of getting back the resource\n");
	for(int i = 0;i<NUMBEROFTHREADS;i++){
		scanf("%lf", &retreivalValues[i]);
	}
	memset(status, 0, sizeof(status));//Initialize the status of all to zero
	printf("\nObtained the intput. Commencing the next stage.....\n\n");
	return resourceValues;
}

void makeSemaphores(int* resourceValues){
	// Define the semaphores for each resource
	int value;
	for(int i = 0;i<NUMBEROFTYPES;i++){
		countingSemaphores[i] = (sem_t*)calloc(1, sizeof(sem_t));
		if(countingSemaphores[i] == NULL){
			perror("Dynamic allocation for the semaphore failed\n");
			exit(1);
		}
		value = sem_init(countingSemaphores[i], 0, resourceValues[i]);
		if(value != 0){
			perror("Unable to initialize semaphore\n");
			exit(1);
		}
	}
	// Define semaphore for each process
	binSemaphore = (sem_t*)calloc(1, sizeof(sem_t));
	if(binSemaphore == NULL){
		perror("Dynamic allocation for binary semaphore failed\n");
		exit(1);
	}
	value = sem_init(binSemaphore, 0, 1);
	if(value != 0){
		perror("Unable to initialize binSemaphore\n");
		exit(1);
	}
	//Define the semaphore for deallocation
	binSemaphore2 = (sem_t*)calloc(1, sizeof(sem_t));
	if(binSemaphore2 == NULL){
		perror("Dynamic allocation for binary semaphore failed\n");
		exit(1);
	}
	value = sem_init(binSemaphore2, 0, 1);
	if(value != 0){
		perror("Unable to initialize binSemaphore\n");
		exit(1);
	}
}

int hasType(int resourceNum, int threadNum){
		int type = (int)pow(2, resourceNum);
		if(status[threadNum]%type == 0){
			return 1;
		}
		else{
			return 0;
		}
}

void showResourcetype(int resourceNum){
	switch(resourceNum){
		case 0:printf("A\n");break;
		case 1:printf("B\n");break;
		case 2:printf("C\n");break;
		case 3:printf("D\n");break;
	}
}

void waitFail(int value, char* string){
	if(value != 0){
		perror(string);
		exit(2);
	}
}

void sigFail(int value, char* string){
	if(value != 0){
		perror(string);
		exit(2);
	}
}

int generateRandom(int range, int lowerBound){
	srand(time(NULL));
	return lowerBound+rand()%range;
}

void getResource(int resourceNum, int* resourceValues, int threadNum){
	// Obtain a resource
	int value;
	printf("Thread %d trying to obtain resource:", threadNum);showResourcetype(resourceNum);
	value = sem_wait(binSemaphore2); // Only one can remove or add a resource
	waitFail(value, "wait failed for binary semaphore for resource\n");
		value = sem_wait(countingSemaphores[resourceNum]);
		waitFail(value, "wait failed for counting semaphore\n");
			inUse[resourceNum]++; // To tell how many are currently using the resource
			resourceValues[resourceNum]--;
			sleep(ACQUIRETIME);
			printf("Thread %d successfully obtained resource:", threadNum);showResourcetype(resourceNum);
			status[threadNum] += (int)pow(2, resourceNum);
		value = sem_post(countingSemaphores[resourceNum]);
		sigFail(value, "signal failed for counting semaphore\n");
	value = sem_post(binSemaphore2);
	sigFail(value, "signal failed for binary semaphore for resource\n");
}

void giveBackResource(int resourceNum, double retreivalPercent, int* resourceValues, int threadNum){
	//give back the resource
	int randNum, value;
	value = sem_wait(binSemaphore2);
	waitFail(value, "wait failed for binary semaphore for resource\n");
		randNum = generateRandom(1, PERCENT);
		double fraction =  ((double)randNum)/PERCENT;
		if(fraction <= retreivalPercent){ // Successfully get back the resource back from the thread
			resourceValues[resourceNum]++;
			printf("Obtraied the resource back from thread:%d\n", threadNum);
		}
		inUse[resourceNum]--;//Releasing hold of the resource
		status[threadNum] -= (int)pow(2, resourceNum);
	value = sem_post(binSemaphore2);
	sigFail(value, "signal failed for binary semaphore for resource\n");
}

int hasDeadlock(int threadNum, int resourceNum, int** properties, int* resourceValues){
	int remainingForCurrent = requirement[threadNum]-status[threadNum];
	int requirementOfOther = 0;
	int hasResourceOtherWants = 0;
	int hasResourceTheTargetWants = 0;
	status[threadNum] += (int)pow(2, resourceNum);//After acquiring the resource
	for(int i = 0;i<NUMBEROFTHREADS;i++){ // Comparison with all the other players
		if(i != threadNum){ // To avoid a deadlock check to itself
			requirementOfOther = requirement[i]-status[i];
			hasResourceOtherWants = status[threadNum]&requirementOfOther;
			hasResourceTheTargetWants = status[i]&remainingForCurrent;
			if(hasResourceTheTargetWants>0 && hasResourceOtherWants>0){ // Both covet resources held by the other
				for(int j = 0;j<NUMBEROFTYPES;j++){ // Check which type of resource has the deadlock
					int type = (int)pow(2, j);
					if(hasResourceTheTargetWants%type == 0 && resourceValues[j] == 0){
						status[threadNum] -= (int)pow(2, resourceNum);
						return 1;
					}
				}
			}
		}
	}
	status[threadNum] -= (int)pow(2, resourceNum);
	return 0; // No deadlock
}

void stopExec(int resourceNum, int* resourceValues ){
	if(inUse[resourceNum] == 0 && resourceValues[resourceNum] == 0){
		printf("Unable to complete execution as we have run out of resource:");
		showResourcetype(resourceNum);
		exit(3);
	}
}

void handleDeadlock(int threadNum, int* resourceValues){
	for(int i = 0;i<NUMBEROFTYPES;i++){
		if(hasType(threadNum, i) == 1){
			resourceValues[i]++;
			inUse[i]--;
			status[i] -= (int)pow(2, i);
		}
	}
}

void routine(void* arg){
	// Stuff that should execute for each process
	int value;
	arglist* args = (arglist*)arg;
	printf("Thread:%d has commenced execution\n", args->threadNum);
	int size = (args->sizes)[args->threadNum]; // get the size for the resource number for the current thread
	int visited[size]; // if shows if the resource has been used
	memset(visited, 0, sizeof(visited));

	int count = 0, deadlocked = 0;
	while(count<size){
		printf("Trying the semaphore of Thread:%d\n", args->threadNum);
		value = sem_wait(binSemaphore);
		waitFail(value, "wait failed for binary semaphore\n");
		printf("Thread:%d entered the deadlock check\n", args->threadNum);
		int resourceNum = generateRandom(size, 0);
			if(visited[resourceNum] == 0){
				count++;
				visited[resourceNum] = 1;
				stopExec(resourceNum, args->resourceValues);
				if(hasDeadlock(args->threadNum, resourceNum, args->properties, args->resourceValues) == 1){
					handleDeadlock(args->threadNum, args->resourceValues); //Release all the resources held
					printf("Thread:%d encountered deadlock\n", args->threadNum);
					deadlocked = 1;
					break;
				}
				else if( (args->resourceValues)[resourceNum] ==  0 ){
					printf("Thread:%d unable to get resource as it is unavailable for resource:", args->threadNum);showResourcetype(resourceNum);
					printf("Processes using the resource:%d\n", inUse[resourceNum]);
					count--;
					visited[resourceNum] = 0;
					value = sem_post(binSemaphore);
					sigFail(value, "signal failed for binary semaphore\n");
					continue;
				}
				else{
					getResource(resourceNum, args->resourceValues, args->threadNum);
				}
			}
		printf("Thread:%d exiting the deadlock check\n", args->threadNum);
		value = sem_post(binSemaphore);
		sigFail(value, "signal failed for binary semaphore\n");
	}

	if(deadlocked == 0){// No deadlock release the values
		value = sem_wait(binSemaphore);
		waitFail(value, "wait failed for binary semaphore\n");
		int resourceNum = 0;
		for(int i = 0;i<size;i++){
			resourceNum = args->properties[args->threadNum][i];
			giveBackResource(resourceNum, args->retreivalValues[args->threadNum], args->resourceValues, args->threadNum);
		}
		value = sem_post(binSemaphore);
		sigFail(value, "signal failed for binary semaphore\n");
	}
	else if(deadlocked == 1){
		printf("Thread:%d exiting the deadlock check\n", args->threadNum);
		value = sem_post(binSemaphore);
		sigFail(value, "signal failed for binary semaphore\n");
	}

	if(args->timesExecuted[args->threadNum]>0){
		if(deadlocked == 0)
			args->timesExecuted[args->threadNum]--;
		else
			printf("Thread:%d completed execution\n", args->threadNum);
		pthread_t thread;
		pthread_create(&thread, NULL, (void*)routine, (void*)arg);
		pthread_join(thread, NULL);
	}
}

void getRequirements(int** properties, int* sizes){
	for(int i = 0;i<NUMBEROFTHREADS;i++){
		for(int j = 0;j<sizes[i];j++){
			requirement[i] += (int)pow(2, properties[i][j]);
		}
	}
}

void initArgList(arglist* temp, int threadNum, int* resourceValues, int* timesExecuted, int** properties, int* sizes, double* retreivalValues){
	printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printf("Initializing the argument list...\n");
	temp->threadNum = threadNum;
	temp->resourceValues = resourceValues;
	temp->timesExecuted = timesExecuted;
	temp->properties = properties;
	temp->sizes = sizes;
	temp->retreivalValues = retreivalValues;
	printf("Initilization successfull..\n\n");
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
}

int main(){
	// Properties of each thread
	int property1[] = {0, 1, 2};
	int property2[] = {1, 2, 3};
	int property3[] = {0, 2, 3};
	int property4[] = {0, 1, 3};
	int property5[] = {0};
	int property6[] = {1};
	int property7[] = {2};
	int property8[] = {3};
	int* properties[] = {property1, property2, property3, property4, property5, property6, property7, property8};
	// Total number of resources required by each process
	int sizes[] = {3, 3, 3, 3, 1, 1, 1, 1};
	getRequirements(properties, sizes);// Set the requirements for the bitmask
	int numberOfTimes, value;
	int timesExecuted[NUMBEROFTHREADS]; // How many times each thread should run
	double retreivalValues[NUMBEROFTHREADS];
	int* resourceValues = init(&numberOfTimes, timesExecuted, retreivalValues); // As we dynamically allocate resource values all point to the same refernce
	makeSemaphores(resourceValues);
	arglist* temp;
	pthread_t threadList[NUMBEROFTHREADS];
	for(int i = 0;i<NUMBEROFTHREADS;i++){
		temp = (arglist*)calloc(1, sizeof(arglist)); //Initialize the struct
		initArgList(temp, i, resourceValues, timesExecuted, properties, sizes, retreivalValues);
		value = pthread_create(&threadList[i], NULL, (void*)routine, (void*)temp);
		if(value != 0){
			perror("Thread creation failed\n");
			exit(1);
		}
	}

	printf("Main:Threads have been spawned successfully...\n Waiting for execution to complete\n\n");
	for(int i = 0;i<NUMBEROFTHREADS;i++){
		pthread_join(threadList[i], NULL);
	}
	printf("execution was successful\n");
	return 0;
}
