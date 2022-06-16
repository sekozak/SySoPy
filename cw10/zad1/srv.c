#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <arpa/inet.h>

#define MAX_PLAYERS 10
#define MAX_MSG_LEN 256

typedef struct{
    char* name;
    int fd;
    int active;
} player;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
player *players[MAX_PLAYERS] = {NULL};
int playersNum = 0;

int findOpponent(int idx);
int addPlayer(char *name, int fd);
int findPlayer(char *name);
void freePlayer(int idx);
void removePlayer(char *name);
int createUnixSocket(char* socketPath);
int createInetSocket(char *port);
int controlMessages(int localSocket, int networkSocket);
void* ping();

int main(int argc, char* argv[]){
    srand(time(NULL));
    if (argc != 3){
        printf("Wrong number of arguments!\n");
        exit(1);
    }
    char *port = argv[1], *socketPath = argv[2];
    int localSocket = createUnixSocket(socketPath);
    int networkSocket = createInetSocket(port);

    pthread_t thread;
    pthread_create(&thread, NULL, &ping, NULL);
    while (1){
        int playerFd = controlMessages(localSocket, networkSocket);
        char buff[MAX_MSG_LEN + 1], *command, *name, *arg;
        read(playerFd, buff, MAX_MSG_LEN);

        printf("%s\n", buff);
        command = strtok(buff, ":");
        arg = strtok(NULL, ":");
        name = strtok(NULL, ":");

        pthread_mutex_lock(&mutex);
        if (strcmp(command, "add") == 0){
            int idx = addPlayer(name, playerFd);

            if (idx == -1){
                send(playerFd, "add:name_taken", MAX_MSG_LEN, 0);
                close(playerFd);
            }
            else if (idx % 2 == 0){
                send(playerFd, "add:no_enemy", MAX_MSG_LEN, 0);
            }
            else{
                int random_num=rand()%100, first, second;
                if (random_num%2 == 0){
                    first = idx;
                    second = findOpponent(idx);
                }
                else{
                    second = idx;
                    first = findOpponent(idx);
                }
                send(players[first]->fd, "add:O", MAX_MSG_LEN, 0);
                send(players[second]->fd, "add:X", MAX_MSG_LEN, 0);
            }
        }
        if (strcmp(command, "move") == 0){
            int move = atoi(arg), player = findPlayer(name);
            sprintf(buff, "move:%d", move);
            send(players[findOpponent(player)]->fd, buff, MAX_MSG_LEN,0);
        }
        if (strcmp(command, "stop") == 0) removePlayer(name);
        if (strcmp(command, "pong") == 0){
            int player = findPlayer(name);
            if (player != -1) players[player]->active = 1;
        }
        pthread_mutex_unlock(&mutex);
    }
}


int addPlayer(char *name, int fd){
    int idx = -1;

    for (int i = 0; i < MAX_PLAYERS; i++){
        if (players[i] != NULL && strcmp(players[i]->name, name) == 0) return idx;
    }
    for (int i = 0; i < MAX_PLAYERS; i++){
        if (players[i] == NULL){
            idx = i;
            break;
        }
    }
    if (idx != -1){
        player *new_player = calloc(1, sizeof(player));
        new_player->name = calloc(MAX_MSG_LEN, sizeof(char));
        strcpy(new_player->name, name);
        new_player->fd = fd;
        new_player->active = 1;

        players[idx] = new_player;
        playersNum++;
    }

    return idx;
}

int findPlayer(char *name){
    for (int i = 0; i < MAX_PLAYERS; i++){
        if (players[i] != NULL && strcmp(players[i]->name, name) == 0) return i;
    }
    return -1;
}

void freePlayer(int idx){
    free(players[idx]->name);
    free(players[idx]);
    players[idx] = NULL;
    playersNum--;

    int opponent = findOpponent(idx);
    if (players[opponent] != NULL){
        printf("Removing opponent: %s\n", players[opponent]->name);
        free(players[opponent]->name);
        free(players[opponent]);
        players[opponent] = NULL;
        playersNum--;
    }
}

int findOpponent(int idx){ return idx%2==0 ? idx+1 : idx-1; }

void removePlayer(char *name){
    int idx = -1;
    for (int i = 0; i < MAX_PLAYERS; i++){
        if (players[i] != NULL && strcmp(players[i]->name, name) == 0) idx = i;
    }
    printf("Removing player: %s\n", name);
    freePlayer(idx);
}

void* ping(){
    while (1){
        printf("*Ping*\n");
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_PLAYERS; i++){
            if (players[i] && ! players[i]->active){
                printf("Unactive\n");
                removePlayer(players[i]->name);
            }
        }

        for (int i = 0; i < MAX_PLAYERS; i++){
            if (players[i]){
                send(players[i]->fd, "Ping: ", MAX_MSG_LEN, 0);
                players[i]->active = 0;
            }
        }
        pthread_mutex_unlock(&mutex);
        sleep(10);
    }
}

int createUnixSocket(char* socketPath){
    int sockFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockFd  == -1){
        printf("ERROR while creating local socket!\n");
        exit(1);
    }
    else printf("Created local socket\n");

    struct sockaddr_un sock_addr;
    memset(&sock_addr, 0, sizeof(struct sockaddr_un));
    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, socketPath);
    unlink(socketPath);

    if ((bind(sockFd, (struct sockaddr *) &sock_addr, sizeof(sock_addr))) == -1){
        printf("ERROR while binding local socket!\n");
        exit(1);
    }
    if ((listen(sockFd, MAX_PLAYERS)) == -1){
        printf("ERROR while listening local socket!\n");
        exit(1);
    }
    printf("Local socket fd: %d\n", sockFd);

    return sockFd;
}

int createInetSocket(char *port){
    int sockFd = socket(AF_INET , SOCK_STREAM, 0);
    if (sockFd  == -1){
        printf("ERROR while creating inet socket!\n");
        exit(1);
    }
    else printf("Created inet socket!\n");
    
    struct sockaddr_in sock_addr;
    sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(atoi(port));
    if ((bind(sockFd, (struct sockaddr *) &sock_addr, sizeof(sock_addr))) == -1){
        printf("ERROR while binding inet socket!\n");
        exit(1);
    }
    if ((listen(sockFd, MAX_PLAYERS)) == -1){
        printf("ERROR while listening inet socket!\n");
        exit(1);
    }
    printf("Inet socket fd: %d\n", sockFd);

    return sockFd;
}

int controlMessages(int localSocket, int networkSocket){
    struct pollfd *fds = calloc(playersNum+2, sizeof(struct pollfd));
    fds[0].fd = localSocket;
    fds[1].fd = networkSocket;
    fds[0].events = POLLIN;
    fds[1].events = POLLIN;

    pthread_mutex_lock(&mutex);
    for (int i=0; i<playersNum; i++){
        fds[i+2].fd = players[i]->fd;
        fds[i+2].events = POLLIN;
    }
    pthread_mutex_unlock(&mutex);

    poll(fds, playersNum+2, -1);
    int retval;
    for (int i=0; i<playersNum+2; i++){
        if (fds[i].revents & POLLIN){
            retval = fds[i].fd;
            break;
        }
    }
    if (retval==networkSocket || retval==localSocket){
        retval=accept(retval, NULL, NULL);
    }
    free(fds);

    return retval;
}