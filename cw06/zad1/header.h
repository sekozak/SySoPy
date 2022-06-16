#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#ifndef Z6_HEADER_H
#define Z6_HEADER_H
#endif

#define STOP 1
#define LIST 2
#define ONE 3
#define ALL 4
#define INIT 5
#define ID 'S'

#define MAX_CLIENTS 7
#define MAX_LEN 128
#define MSG_SIZE sizeof(message) - sizeof(long)


typedef struct {
    long type;
    char text[MAX_LEN];
    int q_id;
    pid_t sender_pid;
    int sender_id;
    int receiver_id;
    time_t send_time;
} message;

typedef struct {
    int client_queue;
    pid_t client_pid;
    int used;
}client;

char* home_path(){
    char* path;
    if ((path=getenv("HOME")) == NULL){
        path=getpwuid(getuid())->pw_dir;
    }
    return path;
}

void send_message(int queue,message* msg){
    if(msgsnd(queue, msg, MSG_SIZE, 0) == -1){
        printf("ERROR while sending message!\n");
        exit(1);
    }
}

void receive_message(int queue, message* msg, long type){
    if(msgrcv(queue, msg, MSG_SIZE, type,0) == -1){
        printf("Error while receiving message!\n");
        exit(1);
    }
}

void delete_queue(int queue){
    msgctl(queue, IPC_RMID,NULL);
}
