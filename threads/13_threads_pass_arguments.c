// https://code-vault.net/lesson/18ec1942c2da46840693efe9b51f9f86
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int primes[10] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};

void *routine(void *arg)
{
    sleep(1);
    int index = *(int *)arg;
    printf("%d ", primes[index]);
    free(arg);
    return NULL;
}

int main(void)
{
    pthread_t th[10];
    int i;
    for (i = 0; i < 10; i++)
    {
        int *a = malloc(sizeof(int));
        *a = i;
        if (pthread_create(&th[i], NULL, &routine, a) != 0)
        {
            perror("Failed to create thread");
            free(a);
        }
    }
    for (i = 0; i < 10; i++)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Failed to join thread");
        }
    }

    return 0;
}