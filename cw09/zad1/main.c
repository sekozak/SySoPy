#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define ElvesNumber 10
#define SizeOfQueue 3

pthread_mutex_t mutex_santa = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_santa   = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_elf = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_waitingElves = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_waitingElves  = PTHREAD_COND_INITIALIZER;

int waitingElves = 0;
int elfQueue[SizeOfQueue];

void* elf(void* arg);
void* santa(void* arg);


int main(){
    srand(time(NULL));

    for (int i=0; i<SizeOfQueue; i++){ elfQueue[i] = -1; }

    pthread_t santa_t;
    pthread_create(&santa_t, NULL, &santa, NULL);

    int* elvesId = calloc(ElvesNumber, sizeof(int));
    pthread_t* elves_t = calloc(ElvesNumber, sizeof(pthread_t));
    for (int i=0; i<ElvesNumber; i++){
        elvesId[i] = i;
        pthread_create(&elves_t[i], NULL, &elf, &elvesId[i]);
    }

    pthread_join(santa_t, NULL);
    for (int i=0; i<ElvesNumber; i++){
        pthread_join(elves_t[i], NULL);
    }

    return 0;
}


void* santa(void* arg){
    while(1){
        pthread_mutex_lock(&mutex_santa);
        while (waitingElves < SizeOfQueue){
            pthread_cond_wait(&cond_santa, &mutex_santa);
        }
        pthread_mutex_unlock(&mutex_santa);

        printf("Mikołaj -> budzę się\n");
        if (waitingElves == SizeOfQueue) {
            printf("Mikołaj -> rozwiązuje problemy elfów: (%d, %d, %d)\n", elfQueue[0], elfQueue[1], elfQueue[2]);
            sleep(rand()%2 + 1);

            pthread_mutex_lock(&mutex_waitingElves);
            waitingElves = 0;
            pthread_cond_broadcast(&cond_waitingElves);
            pthread_mutex_unlock(&mutex_waitingElves);
        }
        printf("Mikolaj -> zasypiam\n\n");
    }

    exit(0);
}

void* elf(void* arg){
    int ID = *((int *) arg);

    while(1){
        sleep(rand()%4 + 2);

        pthread_mutex_lock(&mutex_waitingElves);
        while (waitingElves == SizeOfQueue) {
            printf("Elf -> czeka na powrót elfów, ID: %d\n", ID);
            pthread_cond_wait(&cond_waitingElves, &mutex_waitingElves);
        }
        pthread_mutex_unlock(&mutex_waitingElves);

        pthread_mutex_lock(&mutex_elf);
        if (waitingElves < SizeOfQueue){
            elfQueue[waitingElves++] = ID;
            printf("Elf -> czeka [%d] elfów na Mikołaja, ID: %d\n", waitingElves, ID);
            if (waitingElves == SizeOfQueue){
                printf("Elf -> wybudzam Mikołaja, ID: %d\n", ID);
                pthread_mutex_lock(&mutex_santa);
                pthread_cond_broadcast(&cond_santa);
                pthread_mutex_unlock(&mutex_santa);
            }
        }
        pthread_mutex_unlock(&mutex_elf);

        pthread_mutex_lock(&mutex_waitingElves);
        pthread_cond_wait(&cond_waitingElves, &mutex_waitingElves);
        pthread_mutex_unlock(&mutex_waitingElves);
    }
}