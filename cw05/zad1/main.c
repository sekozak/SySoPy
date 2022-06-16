#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define READ 0
#define WRITE 1


char** execForms(char* l){
    char** execForm=(char**)calloc(256,sizeof(char*));

    int j=0;
    char* tt=strtok(l, "=");
    while( (tt=strtok(NULL, "|"))!=NULL ){execForm[j++]=tt; }
    execForm[j++]=NULL;
    return execForm;
}

char** execToArgs(char* line){
    char** execForm=(char**)calloc(256,sizeof(char*));

    int i=0;
    char* tmp=strtok(line, " ");
    execForm[i++]=tmp;
    while( (tmp=strtok(NULL, " "))!=NULL ){execForm[i++]=tmp; }
    return execForm;
}

int findIndex(char* skladnik){
    int len = strlen(skladnik);
    if(!isspace((unsigned char)*&skladnik[len-1])) return atoi(&skladnik[len-1]);
    return atoi(&skladnik[len-2]);
}



int main(int argc, char *argv[]){
    if( argc!=2 ){
        printf("Zla liczba argumentow!");
        exit(1);
    }
    else if( strstr(argv[1],".txt")==0 ){
        printf("Zly plik!");
        exit(1);
    }
    
    int pipe_in[2];
    int pipe_out[2];
    if( pipe(pipe_out)!=0 ) {
        printf("Error while creating a pipe!\n");
        exit(1);
    }

    FILE* f = fopen(argv[1],"r");
    char* l=(char*)calloc(256,sizeof(char));
    char** lines=(char**)calloc(256,sizeof(char*));
    int linesNumber=0, linesCounter=0;
    while( fgets(l, 256*sizeof(char), f) ){
        if(strstr(l,"=")){
            lines[linesCounter]=l; linesCounter++; 
            l=(char*)calloc(256,sizeof(char));
        }
        else{linesNumber++; }
    }
    fclose(f);

    FILE* file = fopen(argv[1],"r");
    char* line=(char*)calloc(256,sizeof(char));
    linesCounter=0;
    while( fgets(line, 256*sizeof(char), file) ){
        if(!strstr(line,"=")){
            char* tmp=strtok(line, "|");
            int execToDo[20], execId=0, n=0;
            do{
                execToDo[execId++]=findIndex(tmp);
                tmp=strtok(NULL, "|");
            }while( tmp!=NULL );
            for(int k=0; k<execId; k++){
                char * tmpline = calloc(256,sizeof(char));
                strcpy(tmpline,lines[execToDo[k]]);
                char** execs = execForms(tmpline);

                int m=0, i=0;
                while (execs[m] != NULL) {
                    printf("arg%d: %s\n", n + 1, execs[m]);
                    m++;
                    n++;
                }

                for(int p=0; p<m; p++){

                    pid_t pid = fork();

                    if(pid == 0){
                        if(0==execId-1 && 0==m-1){}
                        else if (k==0  && p==0) {
                            close(pipe_out[READ]);
                            dup2(pipe_out[WRITE], STDOUT_FILENO);
                        }
                        else if (k==execId-1 && p==m-1) {
                            close(pipe_out[READ]);
                            close(pipe_out[WRITE]);
                            close(pipe_in[WRITE]);
                            dup2(pipe_in[READ], STDIN_FILENO);
                        }
                        else {
                            close(pipe_in[WRITE]);
                            close(pipe_out[READ]);
                            dup2(pipe_in[READ], STDIN_FILENO);
                            dup2(pipe_out[WRITE], STDOUT_FILENO);
                        }

                        char** args = execToArgs(execs[p]);
                        char* path = args[0];

                        if (execvp(path, args) == -1) {
                            printf("ERROR in exec\n");
                            exit(1);
                        }
                    } 
                    else {         
                        close(pipe_in[WRITE]);
                        pipe_in[READ] = pipe_out[READ];
                        pipe_in[WRITE] = pipe_out[WRITE];

                        if (pipe(pipe_out) != 0) {
                            printf("Error while moving pipe!\n");
                            exit(1);
                        }
                        wait(0);
                    }
                }
            }
            printf("\n");
            linesCounter++;
        }
    }

    // int status = 0;
    // pid_t wpid;
    // while ((wpid = wait(&status)) != -1);

    fclose(file);
}