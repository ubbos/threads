/* Create a C program that receives any number of command line arguments.

 * For each argument, the program will create a thread.

 * Each thread converts any lowercase letters into uppercase letters and prints the initial string and the result.

 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

typedef struct
{
    int id;
    char *s;

} data;

void *upcase(void *arg)
{

    data d = *((data *)arg);

    int len = strlen(d.s);

    char *copy = malloc(sizeof(char) * (len + 1));

    strncpy(copy, d.s, len + 1);

    for (int i = 0; i < len; i++)
    {

        if (copy[i] >= 'a' && copy[i] <= 'z')
        {

            copy[i] += 'A' - 'a';
        }
    }

    printf("Thread %d - initial argument %s - result %s\n", d.id, d.s, copy);

    return copy;
}

int main(int argc, char *argv[])
{

    int size = argc - 1;

    pthread_t *th = malloc(sizeof(pthread_t) * size);

    data *args = malloc(sizeof(data) * size);

    if (size == 0)
    {
        printf("No arguments provided.\n");
        free(th);
        free(args);
        return 0;
    }

    int created = 0;
    for (int i = 0; i < size; i++)
    {

        args[i].id = i;

        args[i].s = argv[i + 1];

        if (0 != pthread_create(&th[i], NULL, upcase, &args[i]))
        {

            perror("Error creating thread");
        }
        else
        {
            created++;
        }
    }

    for (int i = 0; i < created; i++)
    {

        char *res = NULL;

        if (0 != pthread_join(th[i], (void **)&res))
        {

            perror("Error joining thread");
        }
        else
        {

            printf("Thread %d has returned %s\n", i, res);
        }

        if (res != NULL)
        {

            free(res);
        }
    }

    free(args);

    free(th);

    return 0;
}
