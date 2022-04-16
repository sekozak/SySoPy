#include "bibl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>

long double time_sec(clock_t time){
    return (long double)(time) / sysconf(_SC_CLK_TCK);
}

void print_res(clock_t clock_start, clock_t clock_end, struct tms start_tms, struct tms end_tms){
    printf("real %Lf\n", time_sec(clock_end - clock_start));
    printf("user %Lf\n", time_sec(end_tms.tms_utime - start_tms.tms_utime));
    printf("sys  %Lf\n\n", time_sec(end_tms.tms_stime - start_tms.tms_stime));
}

void main(int argc, char *argv[]){
    if( !strcmp("./main",argv[0]) ){
        struct tms start_tms, end_tms, start_tms_tot, end_tms_tot;
        clock_t clock_start, clock_end, total_clock_start, total_clock_end;
        total_clock_start = times(&start_tms_tot);

        int i=1, size=0;
        void** tab=create_table("0");
        struct memoryBlock *block;

        while(i<argc){
            if( !strcmp("./wc_files",argv[i]) ){
                int j=0;
                while( strstr(argv[1+i+j],".txt")!=NULL ){
                    j++;
                    if(i+j+1==argc) break;
                };
                char *args[j];
                for(int k=0;k<j;k++){
                    args[k]=argv[i+k+1];
                };
                i=i+j;

                clock_start = times(&start_tms);
                block=wc_files(j, args);
                clock_end = times(&end_tms);
                printf("WC_FILES EXECUTION TIME\n");
                print_res(clock_start, clock_end, start_tms, end_tms);

                clock_start = times(&start_tms);
                pointerToData(tab,size,block);
                clock_end = times(&end_tms);
                printf("SAVETOBLOCK EXECUTION TIME\n");
                print_res(clock_start, clock_end, start_tms, end_tms);
            }
            else if( !strcmp("./create_table",argv[i]) ){
                i++;
                tab=create_table(argv[i]);
                size=atoi(argv[i]);
            }
            else if( !strcmp("./remove_block",argv[i]) ){ 
                i++;

                clock_start = times(&start_tms);
                remove_block(tab, argv[i]);
                clock_end = times(&end_tms);
                printf("REMOVE_BLOCK EXECUTION TIME\n");
                print_res(clock_start, clock_end, start_tms, end_tms);
            };

            i++;
        };
        total_clock_end = times(&end_tms_tot);
        printf("TOTAL EXECUTION TIME\n");
        print_res(total_clock_start, total_clock_end, start_tms_tot, end_tms_tot);
    };

};