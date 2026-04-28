// https://code-vault.net/lesson/18ec1942c2da46840693efe9b51f4be3
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

void *roll_dice(void *arg)
{
    (void)arg;
    int value = (rand() % 6) + 1;
    int *result = malloc(sizeof(int));
    *result = value;
    printf("Thread result: %d\n", *result);
    return (void *)result;
}

int main(void)
{
    int *res;
    srand(time(NULL));
    pthread_t th;
    if (pthread_create(&th, NULL, &roll_dice, NULL) != 0)
    {
        return 1;
    }
    if (pthread_join(th, (void **)&res) != 0)
    {
        return 2;
    }
    printf("Main res: %d\n", *res);
    printf("Result: %d\n", *res);
    free(res);
    return 0;
}