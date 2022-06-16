#include "shared.h"

int takePizza(table* t);

int main(){
    int semId = getSemId(), tableShmId = getTableShmId();
    table* t = shmat(tableShmId, NULL, 0);

    int type;
    while(true) {
        lockSem(semId, EMPTY_TABLE_SEM);

        lockSem(semId, TABLE_SEM);
        type = takePizza(t);
        printf("Deliver [pid: %d timestamp: %s]  Pobieram pizze: %d. Liczba pizz na stole: %d.\n", getpid(), getCurrentTime(), type, t->pizzas);
        unlockSem(semId, TABLE_SEM);

        unlockSem(semId, FULL_TABLE_SEM);

        sleep(DELIVERY_TIME);

        printf("Deliver [pid: %d timestamp: %s]  Dostarczam pizze: %d.\n", getpid(), getCurrentTime(), type);
        sleep(RETURN_TIME);
    }

}

int takePizza(table* t){
    int type = t->arr[t->takeId];
    t->arr[t->takeId] = -1;
    t->takeId = (t->takeId+1) % TABLE_SIZE;
    t->pizzas--;
    return type;
}