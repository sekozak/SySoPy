#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

int **image, **negativeImage;
int W, H, M, n;

void openImage(char* filename);
void createNegative(char* filename);
void* numbersMethod(void* arg);

void main(int argc, char* argv[]){
    struct timeval stop, start;
    FILE* times = fopen("Times.txt", "a");

    if (argc != 5){
        printf("Wrong number of arguments!\n");
        exit(1);
    }
    n = atoi(argv[1]);
    char *divisionMethod = argv[2];
    if (!strcmp(divisionMethod,"numbers") && !strcmp(divisionMethod,"block")){
        printf("ERROR wrong 2 argumnet [division type]");
        exit(1);
    }
    char *inFile = argv[3];
    char *outFile = argv[4];

    openImage(inFile);

    negativeImage=calloc(H, sizeof(int *));
    for(int i=0; i<H ;i++) negativeImage[i]=calloc(W, sizeof(int *));

    pthread_t *threads = calloc(n, sizeof(pthread_t));
    int *threadNum = calloc(n, sizeof(int));
    
    gettimeofday(&start, NULL);
    for(int i=0; i<n; i++){
        threadNum[i]=i;

        if (strcmp(divisionMethod, "numbers") == 0){
            pthread_create(&threads[i], NULL, &numbersMethod, &threadNum[i]);
        }
        else{
            printf("Method not implemented!");
        }

    }


    fprintf(times, "-------------------------------\n");
    fprintf(times, "Method: %s Threads:%d Size: %dx%d\n", divisionMethod, n, W, H);
    for(int i = 0; i < n; i++) {
        long unsigned int* t;
        pthread_join(threads[i], (void **) &t);
        fprintf(times, "thread: %3d | time: %5lu [μs]\n", i, *t);
    }
    fprintf(times, "-------------------------------\n");

    gettimeofday(&stop, NULL);
    long unsigned int* t = malloc(sizeof(long unsigned int));
    *t = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    fprintf(times, "Total Time: %5lu [μs]\n", *t);
    fprintf(times, "-------------------------------\n\n");

    
    createNegative(outFile);

    for(int i=0; i<H; i++){
        free(image[i]);
        free(negativeImage[i]);
    }
    free(image);
    free(negativeImage);
    fclose(times);
}


void openImage(char* filename){
    FILE* f = fopen(filename, "r");
    if (f == NULL){
        printf("ERROR fopen!\n");
        exit(1);
    }

    char* buf = calloc(256, sizeof(char));
    fgets(buf, 256, f);
    fgets(buf, 256, f);
    if(buf[0]=='#') fgets(buf, 256, f);
    sscanf(buf, "%d %d\n", &W, &H);
    printf("w: %d, h: %d\n", W, H);
    fgets(buf, 256, f);
    sscanf(buf, "%d \n", &M);
    printf("M: %d\n", M);

    image = calloc(H, sizeof(int *));
    for (int i=0; i<H; i++) {
        image[i] = calloc(W, sizeof(int));
    }

    int pixel;
    for (int i=0; i<H; i++){
        for (int j=0; j<W; j++){
            fscanf(f, "%d", &pixel);
            image[i][j] = pixel;
        }
    }

    fclose(f);
}

void createNegative(char* filename) {
    FILE *f = fopen(filename, "w");

    if (f == NULL) {
        printf("ERROR fopen!\n");
        exit(1);
    }

    fprintf(f, "P2\n");
    fprintf(f, "%d %d\n", W, H);
    fprintf(f, "255\n");

    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            fprintf(f, "%d ", negativeImage[i][j]);
        }
        fprintf(f, "\n");
    }

    fclose(f);
}

void* numbersMethod(void* arg){
    struct timeval stop, start;
    gettimeofday(&start, NULL);

    int idx = *((int *) arg);
    int from1 = 128 / n * idx;
    int to1 = (idx != n - 1) ? (128 / n * (idx + 1) ) : 128;
    int from2 = 256-to1;
    int to2 = 256-from1;

    printf("from1-to1 [%d-%d] | from2-to2 [%d-%d]\n", from1, to1, from2, to2);

    int pixel;
    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            pixel = image[i][j];
            if ((pixel>=from1 && pixel<to1) || (pixel>=from2 && pixel<to2)){
                negativeImage[i][j] = 255 - pixel;
            }
        }
    }

    gettimeofday(&stop, NULL);
    long unsigned int* t = malloc(sizeof(long unsigned int));
    *t = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    pthread_exit(t);
}