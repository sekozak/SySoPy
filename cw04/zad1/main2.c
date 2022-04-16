#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>


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

        execl("./exec","exec",s,NULL);
    }

    else if(strcmp(s,"mask")==0 || strcmp(s,"pending")==0){
        sigset_t new_mask, old_mask;
        sigemptyset(&new_mask);
        sigaddset(&new_mask,SIGUSR1);
        if(sigprocmask(SIG_BLOCK, &new_mask, &old_mask)<0){
            perror("Signal block fail!\n");
        }

        printf("Try raise signal!\n");
        raise(SIGUSR1);

        if(sigismember(&new_mask,SIGUSR1)){
            printf("Signal pending!\n");
        }
        else{
            printf("Signal not pending!\n");
        }

        execl("./exec","exec",s,NULL);
    }
}