#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#define MAX_MSG_LEN 256
#define EMPTY 0
#define X 1
#define O -1

typedef struct{
    int moves[9];
    int move;
} gBoard;

typedef enum{
    START,
    MOVE,
    OPPONENT_MOVE,
    WAIT_FOR_OPPONENT,
    WAIT_FOR_MOVE,
    QUIT
} state;

int serverSocket, isPlayerO;
char buff[MAX_MSG_LEN + 1], *name, *command, *mode;
gBoard board;
state currState = START;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

gBoard create_board();
int move(gBoard *board, int position);
int checkWinner(gBoard *board);
void serverObserver();
void startGame();
void stop();
void checkGameStatus();
void draw();
void localConnection(char* path);
void inetConnection(char* port);


int main(int argc, char* argv[]){
    signal(SIGINT, stop);

    if (argc < 4){
        printf("Wrong number of arguments!\n");
        exit(1);
    }
    name = argv[1];

    if (strcmp(argv[2], "unix") == 0){
        char* path = argv[3];
        localConnection(path);
    }
    else if (strcmp(argv[2], "inet") == 0){
        char* port = argv[3];
        inetConnection(port);
    }
    else{
        printf("ERROR choose [inet, unix]\n");
        exit(1);
    }

    char msg[MAX_MSG_LEN];
    sprintf(msg, "add: :%s", name);
    send(serverSocket, msg, MAX_MSG_LEN, 0);
    serverObserver();

    return 0;
}


void serverObserver(){
    int threadStarted = 1;
    while (1)
    {
        read(serverSocket, buff, MAX_MSG_LEN);
        command = strtok(buff, ":");
        mode = strtok(NULL, ":");

        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0)
        {
            if (strcmp(mode, "name_taken") == 0) {
                printf("Can't use this name!\n");
                exit(1);
            }
            currState = START;
            if (threadStarted)
            {
                pthread_t thread;
                pthread_create(&thread, NULL, (void *(*)(void *))startGame, NULL);
                threadStarted = 0;
            }
        }
        else if (strcmp(command, "move")==0) currState = OPPONENT_MOVE;
        else if (strcmp(command, "stop")==0){ currState = QUIT; exit(0); }
        else if (strcmp(command, "Ping")==0){ sprintf(buff, "pong: :%s", name); send(serverSocket, buff, MAX_MSG_LEN, 0); }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
}

int move(gBoard *board, int position){
    if (position < 0 || position > 9 || board->moves[position] != EMPTY) return 0;
    board->moves[position] = board->move;
    board->move *= -1;
    return 1;
}

void startGame(){
    while (1){
        if (currState == START){
            if (strcmp(mode, "no_enemy") == 0) currState = WAIT_FOR_OPPONENT;
            else{
                board = create_board();
                isPlayerO = mode[0] == 'O';
                currState = isPlayerO ? MOVE : WAIT_FOR_MOVE;
            }
        }
        else if (currState == WAIT_FOR_OPPONENT){
            pthread_mutex_lock(&mutex);
            while (currState != START && currState != QUIT) pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
            board = create_board();
            isPlayerO = mode[0] == 'O';
            currState = isPlayerO ? MOVE : WAIT_FOR_MOVE;
        }
        else if (currState == WAIT_FOR_MOVE){
            printf("Waiting for oponent move\n");
            pthread_mutex_lock(&mutex);
            while (currState != OPPONENT_MOVE && currState != QUIT){
                pthread_cond_wait(&cond, &mutex);
            }
            pthread_mutex_unlock(&mutex);
        }
        else if (currState == OPPONENT_MOVE){
            int pos = atoi(mode);
            move(&board, pos);
            checkGameStatus();
            if (currState != QUIT){
                currState = MOVE;
            }
            else draw();
        }
        else if (currState == MOVE){
            int pos;
            draw();
            do{
                printf("[%c] move: ", isPlayerO ? 'O' : 'X');
                scanf("%d", &pos);
                pos--;
            } while (!move(&board, pos));
            draw();

            char buff[MAX_MSG_LEN + 1];
            sprintf(buff, "move:%d:%s", pos, name);
            send(serverSocket, buff, MAX_MSG_LEN, 0);

            checkGameStatus();
            if (currState != QUIT) currState = WAIT_FOR_MOVE;
        }
        else if (currState == QUIT) stop();
    }
}

int checkWinner(gBoard *board){

    for (int x = 0; x < 3; x++){
        if (board->moves[x]==board->moves[x+3] && board->moves[x]==board->moves[x+6]){
            if (board->moves[x] != EMPTY) return board->moves[x];
        }
        if (board->moves[x*3]==board->moves[x*3+1] && board->moves[x*3]==board->moves[x*3+2]){
            if (board->moves[x*3] != EMPTY) return board->moves[x*3];
        }
    }

    if ( board->moves[0]==board->moves[4] && board->moves[0]==board->moves[8] ){
        if (board->moves[0] != EMPTY) return board->moves[0];
    }
    if ( board->moves[2]==board->moves[4] && board->moves[2]==board->moves[6] ){
        if (board->moves[2] != EMPTY) return board->moves[2];
    }
    
    return EMPTY;
}

void checkGameStatus(){
    int end=0, draw=1, winner=checkWinner(&board);

    if (winner != EMPTY){
        if ((isPlayerO==1 && winner == O) || (isPlayerO==0 && winner == X)) printf("Game WON! :)\n");
        else printf("Game LOST! :(\n");
        end = 1;
    }

    for (int i = 0; i < 9; i++){
        if (board.moves[i] == EMPTY){
            draw = 0;
            break;
        }
    }

    if (!end && draw) printf("DRAW! :/\n");
    if (end || draw) currState = QUIT;
}

gBoard create_board(){
    gBoard board = {{EMPTY},-1};
    return board;
}

void draw(){
    char symbol;

    for (int i= 0; i<9; i++){
        if (board.moves[i] == EMPTY) symbol = i + 1 + '0';
        else if (board.moves[i] == O) symbol = 'O';
        else symbol = 'X';
        printf("  %c  ", symbol);
        if(i%3==2) printf("\n");
    }
}

void stop(){
    char buff[MAX_MSG_LEN + 1];
    sprintf(buff, "stop: :%s", name);
    send(serverSocket, buff, MAX_MSG_LEN, 0);
    exit(0);
}

void localConnection(char* path){
    serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un sock_addr;
    memset(&sock_addr, 0, sizeof(struct sockaddr_un));
    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, path);

    if (connect(serverSocket, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == -1) {
        printf("ERROR while connecting to LOCAL socket!\n");
        exit(1);
    }
}

void inetConnection(char* port){
    struct addrinfo *info, hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    getaddrinfo("localhost", port, &hints, &info);
    serverSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

    if (connect(serverSocket, info->ai_addr, info->ai_addrlen) == -1){
        printf("ERROR while connecting to INET socket!\n");
        exit(1);
    }

    freeaddrinfo(info);
}