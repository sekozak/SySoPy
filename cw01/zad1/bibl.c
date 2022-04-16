#include "bibl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct memoryBlock{
    char *data;
};

void** create_table(char *argv){
  if( argv!=NULL ){
    int size=atoi(argv);
    void** ws = calloc(size,sizeof(void*));

    return ws;
  };
  printf("%s","brak argumentu size\n");
  exit(1);
};

int pointerToData(void** tab, int size, struct memoryBlock *block){
    int i=0;
    while(i<size){
        if(tab[i]==NULL){
            tab[i]=block;
            return i;
            break;
        };
        i++;
    };
};


struct memoryBlock* wc_files(int argc, char *argv[]){
  int i=0;
  while(i<argc){
    char *f=(char *) calloc(1,256);
    strcat(f,"wc <");
    strcat(f,argv[i]);
    strcat(f,">>tmpFile.txt");
    system(f);
    free(f);

    i++;
  };
    FILE *tmpFile=fopen("tmpFile.txt","r");
    fseek(tmpFile,0,SEEK_END);
    int size=ftell(tmpFile)+1;
    fseek(tmpFile,0,SEEK_SET);

    struct memoryBlock *block=calloc(1,size);
    char *line=(char *) calloc(1,size), *x=(char *) calloc(1,size);

    while(fgets(x,size,tmpFile)){
        strcat(line,x);
    }
    
    block->data=line;
    // fclose(tmpFile);
    system("rm tmpFile.txt");

    return block;
};


void remove_block(void** tab, char *argv){
    int ix=atoi(argv);
    if(tab[ix]!=NULL){
        free(tab[ix]);
        tab[ix]=NULL;
    };
};

