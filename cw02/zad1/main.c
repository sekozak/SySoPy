#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>

long double time_sec(clock_t time){
    return (long double)(time) / sysconf(_SC_CLK_TCK);
}

void print_res(clock_t clock_start, clock_t clock_end, struct tms start_tms, struct tms end_tms){
    printf("real %Lf\n", time_sec(clock_end - clock_start));
    printf("user %Lf\n", time_sec(end_tms.tms_utime - start_tms.tms_utime));
    printf("sys  %Lf\n\n", time_sec(end_tms.tms_stime - start_tms.tms_stime));
}

char* clean_space(char tab[], int *size) {
    char tab2[*size], tmpTab[*size];
    int flag=0, tab2Size=0, j=0;
    for(int i=0;i<*size;i++){
        if(!isspace((unsigned char)*&tab[i])){flag=1;};
        if( tab[i]==10 ) {
            if(flag){
                tmpTab[j]=tab[i];
                j++;
                for(int k=0;k<j;k++){
                    tab2[tab2Size]=tmpTab[k];
                    tab2Size++;
                }
            }
            flag=0;
            j=0;
        }
        else{
            tmpTab[j]=tab[i];
            j++;
        };
    }
    if(flag){
        for(int k=0;k<j;k++){
            tab2[tab2Size]=tmpTab[k];
            tab2Size++;
        }
    }

    char *res=malloc(tab2Size);
    if(tab2[tab2Size-1]==10){tab2Size--; }
    for(int i=0;i<tab2Size;i++){res[i]=tab2[i]; }
    *size=tab2Size;
    return res;
}

void fcopy(char *from, char *to){
    FILE *fromFile=fopen(from,"rb"), *toFile=fopen(to,"w");
    fseek(fromFile,0,SEEK_END);
    const int size=ftell(fromFile);
    fseek(fromFile,0,SEEK_SET);
    char tab[size];
    int readElements=fread(tab,sizeof(char),size,fromFile);

    int s=size;
    char *tmpTab=clean_space(tab,&s);
    char resTab[s];
    for(int i=0; i<s;i++){resTab[i]=tmpTab[i]; }

    fclose(fromFile);
    int writeElements=fwrite(resTab,sizeof(char),s,toFile);
    fclose(toFile);
}

void copy(char *from, char *to){
    int fromFile=open(from, O_RDONLY), toFile=open(to,O_RDWR | O_CREAT,S_IRWXU);
    const int size = lseek(fromFile,0,SEEK_END);
    lseek(fromFile,0,SEEK_SET);
    char tab[size];
    int readElements=read(fromFile,tab,size);

    int s=size;
    char *tmpTab=clean_space(tab,&s);
    char resTab[s];
    for(int i=0; i<s;i++){resTab[i]=tmpTab[i]; }

    close(fromFile);
    int writeElements=write(toFile,resTab,s);
    close(toFile);
}

void main(int argc, char *argv[]){
    struct tms start_tms, end_tms;
    clock_t clock_start, clock_end;
    char *from=calloc(256,sizeof(char)), *to=calloc(256,sizeof(char));
    if(argc==1){
        printf("%s", "File name copied from: ");
        scanf("%s",from);
        printf("%s", "File name copied to: ");
        scanf("%s",to);
    }
    else if(argc==2){
        from=argv[1];
        printf("%s", "File name copied to: ");
        scanf("%s",to);
    }
    else if(argc==3){
        from=argv[1];
        to=argv[2];
    }
    else{
        printf("%s", "Too many aruguments!\n");
        exit(1);
    };

    if( !strstr(from,".txt") || !strstr(to,".txt") ){
        printf("%s", "NO TXT FILE!\n");
        exit(1);
    };

    printf("COPY RESULTS\n");
    clock_start = times(&start_tms);
    copy(from, to);
    clock_end = times(&end_tms);
    printf("TOTAL COPY EXECUTION TIME\n");
    print_res(clock_start, clock_end, start_tms, end_tms);

    printf("FCOPY RESULTS\n");
    clock_start = times(&start_tms);
    fcopy(from, to);
    clock_end = times(&end_tms);
    printf("TOTAL FCOPY EXECUTION TIME\n");
    print_res(clock_start, clock_end, start_tms, end_tms);
}