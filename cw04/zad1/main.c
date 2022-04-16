#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>

void handler(int signum){
    printf("Event handler for %d | PID: %d | PPID: %d\n",signum,getpid(),getppid());
};

int main(int argc, char *argv[]){
    char *s=calloc(256,sizeof(char));
    if(argc==1){
        printf("%s", "Type: ");
        scanf("%s",s);
    }
    else if(argc==2){
        s=argv[1];

    }
    else{
        printf("%s", "Too many aruguments!\n");
        exit(1);
    };

    
    if(strcmp(s,"ignore")==0){
        signal(SIGUSR1,SIG_IGN);
        printf("Try raise signal!\n");
        raise(SIGUSR1);

        pid_t pid=fork();
        if(pid==0){
            printf("Try raise signal in child!\n");
            raise(SIGUSR1);
        }
        else{
            wait(NULL);
        }
    }

    else if(strcmp(s,"handle")==0){
        signal(SIGUSR1,handler);
        printf("Try raise signal!\n");
        raise(SIGUSR1);

        pid_t pid=fork();
        if(pid==0){
            printf("Try raise signal in child!\n");
            raise(SIGUSR1);
        }
        else{
            wait(NULL);
        }
    }

    else if(strcmp(s,"mask")==0 || strcmp(s,"pending")==0){
        sigset_t new_mask, old_mask;
        sigemptyset(&new_mask);
        sigaddset(&new_mask,SIGUSR1);
        if(sigprocmask(SIG_BLOCK, &new_mask, &old_mask)<0){
            perror("Signal block failed!\n");
        }

        printf("Try raise signal!\n");
        raise(SIGUSR1);

        if(sigismember(&new_mask,SIGUSR1)){
            printf("Signal pending!\n");
        }
        else{
            printf("Signal not pending!\n");
        }

        pid_t pid=fork();
        if(pid==0){
            if(strcmp(s,"mask")==0){
                printf("Try raise signal in child!\n");
                raise(SIGUSR1);
            }
            else{
                printf("We are in child process!\n");
            }

            sigpending(&new_mask);
            if(sigismember(&new_mask,SIGUSR1)){
            printf("Signal pending in child process!\n");
            }
            else{
                printf("Signal not pending in child process!\n");
            }
        }
        else{
            wait(NULL);
        }
    }
}