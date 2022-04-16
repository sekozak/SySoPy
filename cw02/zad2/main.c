#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/times.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

long double time_sec(clock_t time){
    return (long double)(time) / sysconf(_SC_CLK_TCK);
}

void print_res(clock_t clock_start, clock_t clock_end, struct tms start_tms, struct tms end_tms){
    printf("real %Lf\n", time_sec(clock_end - clock_start));
    printf("user %Lf\n", time_sec(end_tms.tms_utime - start_tms.tms_utime));
    printf("sys  %Lf\n\n", time_sec(end_tms.tms_stime - start_tms.tms_stime));
}

char* count(char *c, char tab[], int size) {
    char *res=calloc(2,1), *c2=calloc(1,1);
    int chars=0, lines=0, flag=0;
    for(int i=0;i<size;i++){
        if( c[0]==tab[i] ){flag=1; chars++; };
        if( tab[i]==10 ) {
            if(flag){
                lines++;
            }
            flag=0;
        }
    }
    if(flag){
        lines++;
    }

    res[0]=chars;
    res[1]=lines;
    return res;
}

void fcounter(char *c, char *from){
    FILE *fromFile=fopen(from,"rb");
    fseek(fromFile,0,SEEK_END);
    const int size=ftell(fromFile);
    fseek(fromFile,0,SEEK_SET);
    char tab[size];
    int readElements=fread(tab,sizeof(char),size,fromFile);
    char *res=count(c,tab,size);
    fclose(fromFile);
    printf("Char count: %d \nLine count: %d\n",res[0],res[1]);
}

void counter(char *c, char *from){
    int fromFile=open(from, O_RDONLY);
    const int size = lseek(fromFile,0,SEEK_END);
    lseek(fromFile,0,SEEK_SET);
    char tab[size];
    int readElements=read(fromFile,tab,size);
    char *res=count(c,tab,size);
    close(fromFile);
    printf("Char count: %d \nLine count: %d\n",res[0],res[1]);
}

void main(int argc, char *argv[]){
    struct tms start_tms, end_tms;
    clock_t clock_start, clock_end;
    char *from=calloc(256,sizeof(char)), *c=calloc(2,sizeof(char));

    if(argc==1){
        printf("%s", "Char to find: ");
        scanf("%s",c);
        printf("%s", "File searched in: ");
        scanf("%s",from);
    }
    else if(argc==2){
        c=argv[1];
        printf("%s", "File searched in: ");
        scanf("%s",from);
    }
    else if(argc==3){
        from=argv[2];
        c=argv[1];
    }
    else{
        printf("%s", "Too many aruguments!\n");
        exit(1);
    };

    if( !strstr(from,".txt") ){
        printf("%s", "NO TXT FILE!\n");
        exit(1);
    };

    printf("COUNTER RESULTS\n");
    clock_start = times(&start_tms);
    counter(c, from);
    clock_end = times(&end_tms);
    printf("TOTAL COUNTER EXECUTION TIME\n");
    print_res(clock_start, clock_end, start_tms, end_tms);

    printf("FCOUNTER RESULTS\n");
    clock_start = times(&start_tms);
    fcounter(c, from);
    clock_end = times(&end_tms);
    printf("TOTAL FCOUNTER EXECUTION TIME\n");
    print_res(clock_start, clock_end, start_tms, end_tms);
}