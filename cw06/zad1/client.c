#include <signal.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "header.h"

int server_queue, client_queue, client_id;

void INPUT_handler();

void init();

void run();

void send_LIST();

void send_STOP();

void send_ALL(char* text);

void send_ONE(int receiver_id,char* text);

void STOP_handler();

void delete_client_queue();

void MESSAGE_handler(message* msg);

void LIST_handler(message* msg);

void SIGINT_handler();

int get_receiver_id(char* args);

char* get_message_body(char* args);


int main(){
    atexit(delete_client_queue);
    printf("NEW CLIENT!\n");


    key_t key_s;
    if ((key_s = ftok(home_path(), ID)) == -1){
        printf("ERROR while generating key!\n");
        exit(1);
    }

    if ((server_queue = msgget(key_s, 0)) == -1){
        printf("ERROR while getting access to server queue!\n");
        exit(1);
    }

    key_t key_c;
    if ((key_c = ftok(home_path(), getpid())) == -1){
        printf("ERROR while generating key!\n");
        exit(1);
    }

    if ((client_queue = msgget(key_c,IPC_CREAT | IPC_EXCL | 0666)) == -1){
        printf("ERROR while creating client queue!\n");
        exit(1);
    }


    init();
    run();

    return 0;
}


void init(){
    message msg = {.q_id = client_queue, .type = INIT, .sender_pid = getpid()};
    send_message(server_queue, &msg);

    message server_response;
    receive_message(client_queue, &server_response, INIT);
    client_id = server_response.sender_id;
}

void run(){
    struct msqid_ds queue_stat;

    pid_t pid = fork();
    if(pid == -1){
        printf("ERROR while creating child process!\n");
        exit(1);
    }

    if(pid == 0){
        signal(SIGINT,SIGINT_handler);
        while(1){
            if(msgctl(client_queue, IPC_STAT, &queue_stat) == -1){
                printf("ERROR while getting info about client queue!\n");
                if(errno == EIDRM){
                    printf(" - queue was deleted :(\n");
                }
                exit(1);
            }
            if(queue_stat.msg_qnum){
                message msg;
                receive_message(client_queue, &msg, 0);
                switch (msg.type) {
                    case STOP:
                        STOP_handler();
                        break;
                    case ALL:
                        MESSAGE_handler(&msg);
                        break;
                    case ONE:
                        MESSAGE_handler(&msg);
                        break;
                    case LIST:
                        LIST_handler(&msg);
                        break;
                    default:
                        printf("Command not found, use [STOP,LIST,2ALL,2ONE]\n");
                        exit(1);
                }
            }
        }
    }else{
        while(1){
            INPUT_handler();
        }
    }
}

void delete_client_queue(){
    //printf("CLIENT CLOSED");
    delete_queue(client_queue);
}

void send_LIST(){
    printf("CLIENT send command LIST!\n");
    message msg = {.type = LIST, .sender_id = client_id};
    send_message(server_queue, &msg);
}

void send_STOP(){
    printf("CLIENT send command STOP!\n");
    message msg = {.type = STOP, .sender_id = client_id};
    send_message(server_queue, &msg);
}

void send_ALL(char* text){
    printf("CLIENT send command 2ALL!\n");
    message msg = {
            .type = ALL,
            .sender_id = client_id,
    };
    strcpy(msg.text, text);
    send_message(server_queue, &msg);
}

void send_ONE(int receiver_id,char* text){
    printf("CLIENT send command 2ONE!\n");
    message msg = {
            .type = ONE,
            .sender_id = client_id,
            .receiver_id = receiver_id
    };
    strcpy(msg.text, text);
    send_message(server_queue, &msg);
}

void STOP_handler(){
    printf("Command STOP received!\n");
    kill(getppid(), SIGINT); 
    exit(0);
}

void MESSAGE_handler(message* msg){
    time_t time = msg->send_time;
    struct tm *local_time = localtime(&time);

    if (!local_time) {
        printf("ERROR with ltimel!\n");
        exit(1);
    }
    printf("SenderID: %d\nType: %ld\nMessage content: %s\nDate: %d/%02d/%02d %02d:%02d:%02d\n\n",
           msg->sender_id,
           msg->type,
           msg->text,
           local_time->tm_year + 1900,
           local_time->tm_mon + 1,
           local_time->tm_mday,
           local_time->tm_hour,
           local_time->tm_min,
           local_time->tm_sec
    );
}

void LIST_handler(message* msg){
    printf("\nCLIENTS LIST\n");
    for (int i=0; i<strlen(msg->text); ++i) {
        if(msg->text[i] != '\n'){
            printf("ClientID: %c\n", msg->text[i]);
        }
    }
}

void SIGINT_handler(){
    printf("CLIENT received signal SIGINT and closes!\n");

    send_STOP();
    exit(0);
}

int get_receiver_id(char* args) {
    char* receiver_id_str, receiver_id, *new_args = "";

    if ((receiver_id_str = strtok_r(args, " \0", &new_args)) == NULL) {
        printf("ERROR while getting receiver id!\n");
        return -1;
    }

    if ((receiver_id = atoi(receiver_id_str)) < 0 || errno) {
        printf("ERROR while getting receiver id!\n");
        return -1;
    }

    strcpy(args, new_args);
    return receiver_id;
}

char* get_message_body(char* args) {
    char* text=strtok(args, "\n\0");
    if (text==NULL) {
        printf("ERROR while getting message body!\n");
        return NULL;
    }
    return text;
}

void INPUT_handler(){
    char input[MAX_LEN];
    fgets(input,MAX_LEN, stdin);
    char *args;
    char *command=strtok_r(input, " \0", &args);

    if((strcmp(command, "LIST") == 0)||(strcmp(command, "LIST\n") == 0)){
        send_LIST();
    } 
    else if (strcmp(command, "2ALL") == 0){
        char* text;
        text = strtok(args, "\n\0");
        if(text == NULL){
            printf("ERROR no text in message!\n");
            exit(1);
        }
        send_ALL(text);
    } 
    else if(strcmp(command, "2ONE") == 0){
        int receiver_id=get_receiver_id(args);
        char* text=get_message_body(args);
        if( receiver_id< 0 || !text ){
            printf("ERROR no text or receiver in message!\n");
            exit(1);
        }
        send_ONE(receiver_id, text);
    } 
    else if((strcmp(command, "STOP") == 0)||(strcmp(command, "STOP\n") == 0)){
        send_STOP();
        exit(0);
    }
    else{
        printf("ERROR command not found, use [LIST, STOP, 2ONE, 2ALL]\n");
    }
}