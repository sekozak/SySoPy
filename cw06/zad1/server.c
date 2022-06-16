#include <sys/types.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <mqueue.h>
#include "header.h"

client clients_queues[MAX_CLIENTS];


void stop_client(int client_id);

void init_client(message* msg);

void init_clients_queue();

void stop_server();

void stop_client(int client_id);

void list_clients(int client_id);

void send_to_one(int sender_id, int receiver_id, char* text);

void send_to_all(int sender_id, char* text);

void save_message(message* msg);

void handle_message(message* msg);

void SIGINT_handler();

int server_queue;

int main(){
    atexit(stop_server);
    signal(SIGINT, SIGINT_handler);
    printf("\nSERVER Started...\n");


    key_t key;
    if((key = ftok(home_path(), ID)) == -1){
        printf("ERROR while creating key!\n");
        exit(1);
    }

    if((server_queue = msgget(key, IPC_CREAT | 0666)) == -1){
        printf("ERROR while creating server queue!\n");
        exit(1);
    }

    printf("Server queue ID: %d\n", server_queue);
    init_clients_queue();

    message msg;
    while(1){
        if(msgrcv(server_queue, &msg, MSG_SIZE, -(INIT+1), 0) == -1){
            printf("Receive msg 1 error");
            exit(1);
        }
        handle_message(&msg);
    }

    return 0;
}


void init_client(message* msg){
    int q_id = msg->q_id, flag=1;
    pid_t client_pid = msg->sender_pid;

    for(int i=0; i<MAX_CLIENTS; i++){
        if (clients_queues[i].used == 0){
            client c = {.client_queue = q_id, .client_pid= client_pid, .used=1};
            clients_queues[i] = c;
            printf("SERVER create client id: %d, qid: %d!\n", i, q_id);

            message response = {.type = INIT, .sender_id = i};
            send_message(q_id, &response);
            flag=0;
            break;
        }
    }
    if (flag){
        printf("SERVER is full!\n");
        exit(1);
    }
}

void init_clients_queue(){
    for(int i=0; i<MAX_CLIENTS; i++){
        client c = {c.used = 0};
        clients_queues[i] = c;
    }
}

void stop_server(){
    for(int i=0; i<MAX_CLIENTS; i++){
        if(clients_queues[i].used){
            printf("SERVER send STOP to client qid: %d\n", clients_queues[i].client_queue);
            message msg = { .type = STOP };
            send_message(clients_queues[i].client_queue, &msg);
            stop_client(i);
        }
    }
    delete_queue(server_queue);
    printf("\nSERVER CLOSED\n");
}

void stop_client(int client_id){
    if(clients_queues[client_id].used == 1){
        printf("SERVER disconnect client: %d\n", client_id);
        client c = {.used = 0};
        clients_queues[client_id] = c;
    }
}

void list_clients(int client_id){
    printf("\nCLIENTS LIST:\n");
    char buff[MAX_LEN];
    char temp[32];
    buff[0] = '\0';

    for(int i=0; i<MAX_CLIENTS;i++){
        if(clients_queues[i].used){
            printf("ClientID: %d\n", i);
            sprintf(temp, "%d\n", i);
            strcat(buff, temp);
        }
    }

    message msg = {
            .type = LIST
    };
    strcpy(msg.text, buff);
    send_message(clients_queues[client_id].client_queue, &msg);
}

void send_to_one(int sender_id, int receiver_id, char* text){
    if(clients_queues[receiver_id].used == 0){
        printf("ERROR Client id: %d doesnt exist, cannot send him message!\n", receiver_id);
        exit(1);
    }
    message msg = {
            .type = ONE,
            .sender_id = sender_id,
            .send_time = time(NULL)
    };
    strcpy(msg.text, text);
    send_message(clients_queues[receiver_id].client_queue, &msg);
}

void send_to_all(int sender_id, char* text){
    for(int i=0; i<MAX_CLIENTS; i++){
        if(clients_queues[i].used==1 && i!=sender_id){
            send_to_one(sender_id, i, text);
        }
    }
}

void save_message(message* msg) {
    FILE *file;
    if ((file = fopen("records.txt", "a")) == NULL) {
        printf("ERROR fopen error\n");
        exit(1);
    }

    time_t time_now = time(NULL);
    struct tm *local_time = localtime(&time_now);

    fprintf(file, "Date: %d/%02d/%02d %02d:%02d:%02d\nClientID: %d\nType: %ld\nMessage content: '%s'\n\n",
            local_time->tm_year + 1900,
            local_time->tm_mon + 1,
            local_time->tm_mday,
            local_time->tm_hour,
            local_time->tm_min,
            local_time->tm_sec,
            msg->sender_id,
            msg->type,
            msg->text
    );

    fclose(file);
}

void handle_message(message* msg){
    save_message(msg);

    switch (msg->type) {
        case STOP:
            printf("Command STOP received\n");
            stop_client(msg->sender_id);
            break;
        case LIST:
            printf("Command LIST received\n");
            list_clients(msg->sender_id);
            break;
        case ALL:
            printf("Command 2ALL received\n");
            send_to_all(msg->sender_id, msg->text);
            break;
        case ONE:
            printf("Command 2ONE received\n");
            send_to_one(msg->sender_id, msg->receiver_id, msg->text);
            break;
        case INIT:
            printf("Command INIT received\n");
            init_client(msg);
            break;
        default:
            printf("Command not found!\n");
            exit(1);
    }
}

void SIGINT_handler(){
    printf("SERVER received signal SIGINT and closes!\n");
    exit(0);
}