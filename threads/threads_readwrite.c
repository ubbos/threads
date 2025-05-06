/*

 * Create multiple reader and writer threads.

 * Reader threads will print the value of n

 * Writer threads will increment the value of n

 */

#include <stdio.h>

#include <stdlib.h>

#include <pthread.h>

#include <string.h>

#include <unistd.h>

typedef struct
{

    int *n;

    pthread_rwlock_t *rw;

} data;

void *w(void *arg)
{

    data d = *((data *)arg);

    for (int i = 0; i < 1000; i++)
    {

        pthread_rwlock_wrlock(d.rw);

        *(d.n) += 1;

        pthread_rwlock_unlock(d.rw);
    }

    return NULL;
}

void *r(void *arg)
{

    data d = *((data *)arg);

    for (int i = 0; i < 10; i++)
    {

        pthread_rwlock_rdlock(d.rw);

        printf("Current value is: %d\n", *(d.n));

        pthread_rwlock_unlock(d.rw);
    }

    return NULL;
}

int main(int argc, char *argv[])
{

    int r_size = 10;

    int w_size = 2;

    pthread_t *r_th = malloc(sizeof(pthread_t) * r_size);

    pthread_t *w_th = malloc(sizeof(pthread_t) * w_size);

    data *r_args = malloc(sizeof(data) * r_size);

    data *w_args = malloc(sizeof(data) * w_size);

    int *n = malloc(sizeof(int));

    *n = 0;

    pthread_rwlock_t *rw = malloc(sizeof(pthread_rwlock_t));

    pthread_rwlock_init(rw, NULL);

    for (int i = 0; i < w_size; i++)
    {

        w_args[i].n = n;

        w_args[i].rw = rw;

        if (0 != pthread_create(&w_th[i], NULL, w, &w_args[i]))
        {

            perror("Error creating writer thread");
        }
    }

    for (int i = 0; i < r_size; i++)
    {

        r_args[i].n = n;

        r_args[i].rw = rw;

        if (0 != pthread_create(&r_th[i], NULL, r, &r_args[i]))
        {

            perror("Error creating reader thread");
        }
    }

    for (int i = 0; i < w_size; i++)
    {

        if (0 != pthread_join(w_th[i], NULL))
        {

            perror("Error joining writer thread");
        }
    }

    for (int i = 0; i < r_size; i++)
    {

        if (0 != pthread_join(r_th[i], NULL))
        {

            perror("Error joining reader thread");
        }
    }

    printf("Final value = %d\n", *n);

    pthread_rwlock_destroy(rw);

    free(rw);

    free(n);

    free(w_args);

    free(w_th);

    free(r_args);

    free(r_th);

    return 0;
}
