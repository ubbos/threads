#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#define SIZE 5
typedef struct {
    pthread_mutex_t *mtx;
    pthread_cond_t *cond;
    int *arr;
    int *flag;
} data;

void *f1(void *arg) {
    data d = *((data *) arg);
    while(1) {
        pthread_mutex_lock(d.mtx);
        if(*(d.flag) == 2) {
            pthread_mutex_unlock(d.mtx);
            break;
        }
        while(*(d.flag) != 0) {
            pthread_cond_wait(d.cond, d.mtx);
        }
        if(*(d.flag) == 2) {
            pthread_mutex_unlock(d.mtx);
            break;
        }
        printf("Thread 1: ");
        for(int i = 0; i < SIZE; i++) {
            d.arr[i] = rand() % 1000;
            printf("%4d", d.arr[i]);
        }
        printf("\n");
        *(d.flag) = 1;
        pthread_cond_signal(d.cond);
        pthread_mutex_unlock(d.mtx);
    }
    return NULL;
}

void *f2(void *arg) {
    data d = *((data *) arg);
    while(1) {
        pthread_mutex_lock(d.mtx);
        while(*(d.flag) != 1) {
            pthread_cond_wait(d.cond, d.mtx);
        }
        *(d.flag) = 2;
        for(int i = 0; i < SIZE - 1; i++) {
            if(d.arr[i] > d.arr[i+1]) {
                *(d.flag) = 0;
                break;
            }
        }
        if(*(d.flag) == 2) {
            printf("Thread 2: ");
            for(int i = 0; i < SIZE; i++) {
                printf("%4d", d.arr[i]);
            }
            printf("\n");
            pthread_cond_signal(d.cond);
            pthread_mutex_unlock(d.mtx);
            break;
        }
        pthread_cond_signal(d.cond);
        pthread_mutex_unlock(d.mtx);
    }
    return NULL;
}

int main() {
    pthread_t th[2];
    data arg;
    pthread_mutex_t mtx;
    pthread_cond_t cond;
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&cond, NULL);
    int arr[SIZE];
    int flag = 0;
    srand(getpid());
    arg.mtx = &mtx;
    arg.cond = &cond;
    arg.flag = &flag;
    arg.arr = arr;

    pthread_create(&th[0], NULL, f1, &arg);
    pthread_create(&th[1], NULL, f2, &arg);
    for(int i = 0; i < 2; i++) {
        pthread_join(th[i], NULL);
    }
    return 0;
}