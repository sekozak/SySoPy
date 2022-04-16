#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


int main(int argc, char *argv[]){
    printf("We are in (exec) child process!\n");

    char *s=calloc(256,sizeof(char));
    s=argv[1];
    if(strcmp(s,"ignore")==0){
        printf("Try raise signal in child!\n");
        raise(SIGUSR1);
    }

    else if(strcmp(s,"mask")==0 || strcmp(s,"pending")==0){

        if(strcmp(s,"mask")==0){
                printf("Try raise signal in child!\n");
                raise(SIGUSR1);
        }

        sigset_t new_mask;
        sigpending(&new_mask);
        if(sigismember(&new_mask,SIGUSR1)){
        printf("Signal pending in child process!\n");
        }
        else{
            printf("Signal not pending in child process!\n");
        }
    }
}