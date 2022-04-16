#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <math.h>
#include <sys/wait.h>

float f(float x){
    return 4/(x*x+1);
}

int compare_float(float f1, float f2)
 {
  float precision = 0.000001;
  if (((f1 - precision) < f2) && 
      ((f1 + precision) > f2))
   {
    return 1;
   }
  else
   {
    return 0;
   }
 }

float count_some(int n,float width ,float start, float end){
    float sum=0;
    int s=start/width;

    if( !compare_float(start,s*width) ){s++;}
    start=s*width;
    
    while(start<end && start+width<1.0){
        if( compare_float(start,end) ){break;}
        sum+=width*f(start+width/2);
        start+=width;
    }

    char *name=(char*)calloc(256,sizeof(char));
    char *num=(char*)calloc(256,sizeof(char));
    sprintf(num,"%d",n+1);
    strcat(name,"w");
    strcat(name,num);
    strcat(name,".txt");

    FILE *result=fopen(name,"w");
    fprintf(result,"%f",sum);
    fclose(result);

    return sum;
}

float count_res(int n){
    float sum=0;

    for(int i=1; i<=n; i++){

        char *name=(char*)calloc(256,sizeof(char));
        char *num=(char*)calloc(256,sizeof(char));
        sprintf(num,"%d",i);
        strcat(name,"w");
        strcat(name,num);
        strcat(name,".txt");

        float tmp=0.0;
        FILE *result=fopen(name,"r");
        fscanf(result,"%f",&tmp);
        fclose(result);
        sum+=tmp;
    }

    return sum;
}

long double time_sec(clock_t time){
    return (double)(time) / sysconf(_SC_CLK_TCK);
}

void print_res(clock_t clock_start, clock_t clock_end, struct tms start_tms, struct tms end_tms){
    printf("real %Lf\n", time_sec(clock_end - clock_start));
    printf("user %Lf\n", time_sec(end_tms.tms_utime - start_tms.tms_utime));
    printf("sys  %Lf\n\n", time_sec(end_tms.tms_stime - start_tms.tms_stime));
}

int main(int argc, char *argv[]){
    struct tms start_tms, end_tms;
    clock_t clock_start, clock_end;
    char *s=calloc(256,sizeof(char));
    int n;
    float width, result;
    if(argc==1){
        printf("%s", "Szrokosc: ");
        scanf("%s",s);
        width=strtod(s,NULL);
        printf("%s", "N: ");
        scanf("%s",s);
        n=atoi(s);
    }
    else if(argc==2){
        width=strtod(argv[1],NULL);
        printf("%s", "N: ");
        scanf("%s",s);
        n=atoi(s);
    }
    else if(argc==3){
        width=strtod(argv[1],NULL);
        n=atoi(argv[2]);
    }
    else{
        printf("%s", "Too many aruguments!\n");
        exit(1);
    };

    if(width>1){
        printf("%s", "Too big first argument!\n(should be 0<x<1)\n");
        exit(1);
    }
    clock_start = times(&start_tms);

    for(float i=0; i<n; i++){
        int pid=fork();
        if(pid==0){
            count_some(i,width,i/n,(i+1)/n);
            return 0;
        }
    };
    while( wait(0) > 0 );
    
    result=count_res(n);
    clock_end = times(&end_tms);
    printf("\n%lf\n",result);
    print_res(clock_start, clock_end, start_tms, end_tms);

}