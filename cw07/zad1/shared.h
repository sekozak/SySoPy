#ifndef LAB07_SHARED_H
#define LAB07_SHARED_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define ID 'S'
#define OVEN_ID 'O'
#define TABLE_ID 'T'
#define TABLE_PATH "table"
#define OVEN_PATH "oven"

#define OVEN_SIZE 5
#define TABLE_SIZE 5
#define OVEN_SEM 0
#define TABLE_SEM 1
#define FULL_OVEN_SEM 2
#define FULL_TABLE_SEM 3
#define EMPTY_TABLE_SEM 4

#define PREPARATION_TIME 1
#define BAKING_TIME 4
#define DELIVERY_TIME 4
#define RETURN_TIME 4

char* getHomePath(){
    char* path;
    if ((path= getenv("HOME")) == NULL) {
        path = getpwuid(getuid())->pw_dir;
    }
    return path;
}

typedef struct{
    int arr[OVEN_SIZE];
    int putId;
    int takeId;
    int pizzas;
} oven;

typedef struct{
    int arr[TABLE_SIZE];
    int putId;
    int takeId;
    int pizzas;
} table;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
} arg;


void lockSem(int sem_id, int sem_num){
    struct sembuf s = {.sem_num = sem_num, .sem_op = -1};
    if (semop(sem_id, &s, 1) == -1){
        printf("ERROR lockSem!\n");
        exit(1);
    }
}

void unlockSem(int sem_id, int sem_num) {
    struct sembuf s = {.sem_num = sem_num, .sem_op = 1};
    if (semop(sem_id, &s, 1) == -1) {
        printf("ERROR unlockSem!\n");
        exit(1);
    }
}

int getSemId(){
    key_t key = ftok(getHomePath(), ID);
    int semId;
    if((semId = semget(key, 5,0)) == -1){
        printf("ERROR getSemId!\n");
        exit(1);
    }
    return semId;
}

int getTableShmId(){
    key_t key_t = ftok(TABLE_PATH, TABLE_ID);
    int tableShmId;
    if ((tableShmId = shmget(key_t, sizeof(table), 0)) == -1){
        printf("ERROR getTableShmId!\n");
        exit(1);
    }
    return tableShmId;
}

int getOvenShmId(){
    key_t key_o = ftok(OVEN_PATH, OVEN_ID);
    int ovenShmId;
    if ((ovenShmId = shmget(key_o, sizeof(oven), 0)) == -1){
        printf("ERROR getOvenShmId!\n");
        exit(1);
    }
    return ovenShmId;
}

char* getCurrentTime(){
    struct timeval cur_time;
    gettimeofday(&cur_time, NULL);
    int millisec = cur_time.tv_usec / 1000;

    char* buffer = calloc(80, sizeof(char));
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&cur_time.tv_sec));

    char* current_time = calloc(84, sizeof(char));
    sprintf(current_time, "%s:%03d", buffer, millisec);
    return current_time;
}

#endif //LAB07_SHARED_H