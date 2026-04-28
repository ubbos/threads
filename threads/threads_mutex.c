/*

 * Increment a variable 1000000 times using 1000 threads. Each thread should increment the variable 1000 times.

 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

typedef struct
{

    int *n;

    pthread_mutex_t *mtx;

} data;

void *f(void *arg)
{

    data d = *((data *)arg);

    for (int i = 0; i < 1000; i++)
    {

        pthread_mutex_lock(d.mtx);

        *(d.n) += 1;

        pthread_mutex_unlock(d.mtx);
    }

    return NULL;
}

int main(int argc, char *argv[])
{

    int size = 1000;

    pthread_t *th = malloc(sizeof(pthread_t) * size);

    data *args = malloc(sizeof(data) * size);

    int *n = malloc(sizeof(int));

    *n = 0;

    pthread_mutex_t *mtx = malloc(sizeof(pthread_mutex_t));

    pthread_mutex_init(mtx, NULL);

    for (int i = 0; i < size; i++)
    {

        args[i].n = n;

        args[i].mtx = mtx;

        if (0 != pthread_create(&th[i], NULL, f, &args[i]))
        {
            perror("Error creating thread");
            size = i; // only join successfully created threads
            break;
        }
    }

    for (int i = 0; i < size; i++)
    {

        if (0 != pthread_join(th[i], NULL))
        {

            perror("Error joining thread");
        }
    }

    printf("Final value = %d\n", *n);

    pthread_mutex_destroy(mtx);

    free(mtx);

    free(n);

    free(args);

    free(th);

    return 0;
}
