#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>
#include<sys/wait.h>
#include<errno.h>
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int flag=0;
void handler(int s){
    //signal(SIGINT,handler);
    printf("\ntype quit to exit %d\n",s);
    flag=1;
    fflush(stdout);
    return;
}

void getCurrentDirectory(char* currentDirectory){
    int fd[2];
    pipe(fd);
    int cid = fork();
    if(cid == 0){
        close(fd[0]);
        dup2(fd[1],1);
        char* str[] = {"pwd",NULL};
        execvp("pwd",str);
        close(fd[0]);
        exit(EXIT_SUCCESS);
    }
    else{
        char buf;
        wait(NULL);
        close(fd[1]);
        int i=0;
        while(i<100 && read(fd[0],&buf,1)>0)
            currentDirectory[i++] = buf;
        currentDirectory[i-1] = '\0';
        close(fd[0]);
    }
}

void changeDirectory(int numberOfArgs, char** args){
   if(numberOfArgs == 1){
        char cwd[100];
        memset(cwd,0,sizeof(cwd));
        getCurrentDirectory(cwd);
        printf("%s\n",cwd);
    }
    else{
        char str[100];
        args[1] = strtok(args[1],"\n");
        getCurrentDirectory(str);
        strcat(str,"/");
        strcat(str,args[1]);
        printf("%s\n",str);
        int result = chdir(args[1]);
      if(result == 0){
        printf(ANSI_COLOR_GREEN "directory changed\n"  ANSI_COLOR_RESET);
      }

      else{
        switch(errno){
          case EACCES: printf(ANSI_COLOR_RED "Permission denied" ANSI_COLOR_RESET);
          break;
          case EIO:  printf(ANSI_COLOR_RED "An input output error occured" ANSI_COLOR_RESET);
          break;
          case ENAMETOOLONG: printf(ANSI_COLOR_RED "Path is to long" ANSI_COLOR_RESET);
          break;
          case ENOTDIR: printf(ANSI_COLOR_RED "A component of path not a directory" ANSI_COLOR_RESET);
          break;
          case ENOENT: printf( ANSI_COLOR_RED "No such file or directory" ANSI_COLOR_RESET); break;

          default: printf(ANSI_COLOR_RED "Couldn't change directory to %s" ANSI_COLOR_RESET, args[1] );
        }
        printf("\n");
      }

    }
}

void echoFunction(int numberOfArgs, char** args){
    int cid = fork();
    //args[numberOfArgs-1] = NULL;
    if(cid == 0){

        char* newArgs[numberOfArgs+1];
        for(int i=0;i<numberOfArgs;i++){
            newArgs[i] = args[i];
        }
        newArgs[numberOfArgs-1] = strtok(newArgs[numberOfArgs-1],"\n");
        newArgs[numberOfArgs] = NULL;
        execvp("/bin/echo",newArgs);

        exit(EXIT_SUCCESS);
    }
    else{
        wait(NULL);

    }
}

void lsFunction(int numberOfArgs, char** args){
    printf("IN LS\n");
    int cid = fork();
    //args[numberOfArgs-1] = NULL;
    if(cid == 0){
        // if(numberOfArgs == 1){
        //     execlp("/bin/ls","/bin/ls",NULL);
        // }
        //else{

            char* newArgs[numberOfArgs+1];
            for(int i=0;i<numberOfArgs;i++){
                newArgs[i] = args[i];
            }
            newArgs[numberOfArgs-1] = strtok(newArgs[numberOfArgs-1],"\n");
            newArgs[numberOfArgs] = NULL;
            execvp("/bin/ls",newArgs);
        //}

        exit(EXIT_SUCCESS);
    }
    else{
        wait(NULL);

    }
}

void wcFunction(int numberOfArgs, char** args){
    if(numberOfArgs == 1){
        printf(ANSI_COLOR_RED "Insufficient number of arguments, include options and/or file\n" ANSI_COLOR_RESET);
    }
    else if(numberOfArgs == 2){
        // printf("%s %s\n",args[0],args[1]);
        // if(args[1][0] != '-'){

            int cid = fork();
            if(cid == 0){
                args[1] = strtok(args[1],"\n");
                execlp("/usr/bin/wc","/usr/bin/wc",args[1],NULL);
                exit(EXIT_SUCCESS);
            }
            else{
                wait(NULL);
            }
        // }
        // else{
        //     printf(ANSI_COLOR_RED "Incorrect file/directory name format \n" ANSI_COLOR_RESET);
        // }
    }
    else if(numberOfArgs == 3){
        int cid = fork();
            if(cid == 0){
                args[2] = strtok(args[2],"\n");
                execlp("/usr/bin/wc","/usr/bin/wc",args[1],args[2],NULL);
                exit(EXIT_SUCCESS);
            }
            else{
                wait(NULL);
            }
    }
    else{
        printf(ANSI_COLOR_RED "Too many arguments, syntax wc <option> <file> \n" ANSI_COLOR_RESET);
    }
}

int main(){
    char* bashPrompt = "user@ubuntu";
    char currentDirectory[100];
    char dollar = '$';
    memset(currentDirectory,0,sizeof(currentDirectory));
    char test[100];
    signal(SIGINT,handler);
    char* wrongArgs = "Wrong number of arguments passed";
    int count=0;
    while(1){
        count++;
        flag=0;
        getCurrentDirectory(currentDirectory);
        printf(ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET ":" ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET "%c ",bashPrompt,currentDirectory,dollar);
        if(flag==1){
            continue;
        }
        else{
            fgets(test,sizeof(test),stdin);
        }
        printf("Got input %d %s\n",count,test);
        char *temp2 = (char*)malloc(100*sizeof(char));
        memset(temp2,0,sizeof(temp2));
        strcpy(temp2,test);
       char* temp = strtok(test," \t");
        int numberOfArgs = 1;
        while(temp != NULL){
            temp = strtok(NULL," \t");
            if(temp != NULL)
                numberOfArgs++;
        }
        printf("No. of Arguments passed is %d\n",numberOfArgs);
        temp = strtok(temp2," \t");
        char* args[numberOfArgs];
        int i = 0;
        while(temp != NULL){
            args[i++] = temp;
            temp = strtok(NULL," \t");
        }
        if(numberOfArgs == 1){
            if(strcmp(args[0],"\n") == 0)
                continue;
            else
                strcpy(args[0],strtok(args[0],"\n"));
        }
        if(strcmp(args[0],"quit") == 0 || strcmp(args[0],"cd") == 0 || strcmp("echo",args[0]) == 0 || strcmp("ls",args[0])== 0 || strcmp(args[0],"wc") == 0){
            if(strcmp(args[0],"quit") == 0) exit(0);
            else if(strcmp(args[0],"cd") == 0){
                if(numberOfArgs >= 3) {printf("%s\n", wrongArgs); continue;}
                changeDirectory(numberOfArgs,args);
            }
             else if(strcmp(args[0],"echo") == 0){
                echoFunction(numberOfArgs,args);
             }
             else if(strcmp(args[0],"ls") == 0) lsFunction(numberOfArgs,args);
             else wcFunction(numberOfArgs,args);
        }
        else{
            printf(ANSI_COLOR_RED "Wrong command entered, try cd <directory>\nwc <option> <file>\nls <option>\necho <comment>\nquit to exit\n" ANSI_COLOR_RESET);
            continue;
        }
    }
    return 0;
}
