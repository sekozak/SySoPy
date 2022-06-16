#include "shared.h"

int shmOvenId, shmTableId, semId;

void initOven(oven* o);

void initTable(table* t);

void createSems();

void createShm();

void SIGINT_handler();


int main(int argc, char* argv[]){
    signal(SIGINT, SIGINT_handler);
    atexit(SIGINT_handler);

    if (argc != 3){
        printf("Wrong number of arguments!\n");
        exit(1);
    }
    int chiefs = atoi(argv[1]), deliveries = atoi(argv[2]);

    createShm();
    createSems();

    for (int i=0; i<chiefs; i++){
        pid_t pid = fork();
        if (pid == 0){
            execl("./chief", "./chief", NULL);
        }
    }
    for (int i=0; i<deliveries; i++){
        pid_t pid = fork();
        if (pid == 0){
            execl("./delivery", "./delivery", NULL);
        }
    }
    for(int i=0; i<chiefs+deliveries; i++) wait(NULL);

    return 0;
}


void initOven(oven* o){
    for (int i = 0; i < OVEN_SIZE; i++){
        o->arr[i] = -1;
    }
    o->putId = 0;
    o->takeId = 0;
    o->pizzas = 0;  
}

void initTable(table* t){
    for (int i = 0; i < OVEN_SIZE; i++){
        t->arr[i] = -1;
    }
    t->putId = 0;
    t->takeId = 0;
    t->pizzas = 0; 
}

void createShm(){
    key_t key_o, key_t;

    if ((key_o = ftok(OVEN_PATH, OVEN_ID)) == -1){
        printf("ERROR while generating oven key!\n");
        exit(1);
    }
    if ((key_t = ftok(TABLE_PATH, TABLE_ID)) == -1){
        printf("ERROR while generating table key!\n");
        exit(1);
    }
    if ((shmOvenId = shmget(key_o, sizeof(oven), IPC_CREAT | 0666)) == -1){
        printf("ERROR while creating oven shm!\n");
        exit(1);
    }
    if ((shmTableId = shmget(key_t, sizeof(table), IPC_CREAT | 0666)) == -1){
        printf("ERROR while creating table shm!\n");
        exit(1);
    }

    oven* o = shmat(shmOvenId, NULL, 0);
    initOven(o);
    table* t = shmat(shmTableId, NULL, 0);
    initTable(t);

    printf("Shm oven_id: %d\nShm table_id: %d \n", shmOvenId, shmTableId);
}

void createSems(){
    key_t key;

    if ((key = ftok(getHomePath(), ID)) == -1){
        printf("ERROR while generating key!\n");
        exit(1);
    }
    if((semId = semget(key, 5, 0666 | IPC_CREAT)) == -1){
        printf("ERROR while creating semaphore set!\n");
        exit(1);
    }

    union arg;
    arg.val = 1;

    if (semctl(semId, OVEN_SEM, SETVAL, arg) == -1){
        printf("ERROR while setting OVEN_SEM!\n");
        exit(1);
    }
    if (semctl(semId, TABLE_SEM, SETVAL,arg) == -1){
        printf("ERROR while setting OVEN_SEM!\n");
        exit(1);
    }

    arg.val = OVEN_SIZE;
    if (semctl(semId, FULL_OVEN_SEM, SETVAL,arg) == -1){
        printf("ERROR while setting FULL_OVEN_SEM!\n");
        exit(1);
    }

    arg.val = TABLE_SIZE;
    if (semctl(semId, FULL_TABLE_SEM, SETVAL,arg) == -1){
        printf("ERROR while setting FULL_TABLE_SEM!\n");
        exit(1);
    }

    arg.val = 0;
    if (semctl(semId, EMPTY_TABLE_SEM, SETVAL,arg) == -1){
        printf("ERROR while setting EMPTY_TABLE_SEM!\n");
        exit(1);
    }

    printf("Created semId: %d\n", semId);
}

void SIGINT_handler(){
    semctl(semId, 0, IPC_RMID, NULL);
    shmctl(shmTableId, IPC_RMID, NULL);
    shmctl(shmOvenId, IPC_RMID, NULL);
}