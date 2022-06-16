#include "shared.h"

int takePizzaFromOven(oven* o);

void putPizzaToOven(oven* o, int type);

void putPizzaOnTable(table* t, int type);

int main(){
    int semId=getSemId(), ovenShmId=getOvenShmId(), tableShmId=getTableShmId();
    oven* o = shmat(ovenShmId, NULL, 0);
    table* t = shmat(tableShmId, NULL, 0);

    int type;
    srand(getpid());
    while(true){
        type = rand() % 10;
        printf("Chief [pid: %d timestamp: %s]  Przygotowuje pizze: %d\n", getpid(), getCurrentTime(), type);
        sleep(PREPARATION_TIME);

        lockSem(semId, FULL_OVEN_SEM);

        lockSem(semId, OVEN_SEM);
        putPizzaToOven(o, type);
        printf("Chief [pid: %d timestamp: %s]  Dodałem pizze: %d. Liczba pizz w piecu: %d.\n", getpid(), getCurrentTime(), type, o->pizzas);
        unlockSem(semId, OVEN_SEM);

        sleep(BAKING_TIME);

        lockSem(semId, OVEN_SEM);
        type = takePizzaFromOven(o);
        printf("Chief [pid: %d timestamp: %s]  Wyjalem pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", getpid(), getCurrentTime(), type, o->pizzas, t->pizzas);
        unlockSem(semId, OVEN_SEM);

        unlockSem(semId, FULL_OVEN_SEM);

        lockSem(semId, FULL_TABLE_SEM);

        lockSem(semId, TABLE_SEM);
        putPizzaOnTable(t, type);
        printf("Chief [pid: %d timestamp: %s]  Umieszczam pizze na stole: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", getpid(), getCurrentTime(), type, o->pizzas, t->pizzas);
        unlockSem(semId, TABLE_SEM);

        unlockSem(semId, EMPTY_TABLE_SEM);
    }

}

void putPizzaToOven(oven* o, int type){
    o->arr[o->putId] = type;
    o->putId = (o->putId+1) % OVEN_SIZE;
    o->pizzas++;
}

void putPizzaOnTable(table* t, int type){
    t->arr[t->putId] = type;
    t->putId = (t->putId+1) % OVEN_SIZE;
    t->pizzas++;
}

int takePizzaFromOven(oven* o){
    int type = o->arr[o->takeId];
    o->arr[o->takeId] = -1;
    o->takeId = (o->takeId+1) % OVEN_SIZE;
    o->pizzas--;
    return type;
}