#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>

int main(int argc, char *argv[]){
    char *s=calloc(256,sizeof(char));
    int n;
    if(argc==1){
        printf("%s", "N: ");
        scanf("%s",s);
        n=atoi(s);
    }
    else if(argc==2){
        n=atoi(argv[1]);
    }
    else{
        printf("%s", "Too many aruguments!\n");
        exit(1);
    };

    for(int i=0; i<n; i++){
        int pid=fork();
        if(pid==0){
            printf("Proces: %d\n", i);
            return 0;
        }
    };
}