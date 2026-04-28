//https:// code-vault.net/lesson/18ec1942c2da46840693efe9b51ea1a2
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int mails = 0;

void *routine(void *arg)
{
    for (int i = 0; i < 1000000; i++)
    {
        mails++;
        // read mails
        // increment
        // write mails
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t p1, p2;
    if (pthread_create(&p1, NULL, &routine, NULL) != 0)
    {
        return 1;
    }
    if (pthread_create(&p2, NULL, &routine, NULL) != 0)
    {
        return 2;
    }
    if (pthread_join(p1, NULL) != 0)
    {
        return 3;
    }
    if (pthread_join(p2, NULL) != 0)
    {
        return 4;
    }
    printf("Number of mails: %d\n", mails);
    return 0;
}
