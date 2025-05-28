#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef struct {
    FILE *f;
    pthread_mutex_t *mtx_file, *mtx_vars;
    pthread_barrier_t *bar;
    int *lines, *words, *chars;
} data;

void *fun(void *arg) {
    data d = *((data *) arg);
    // wait for all threads to get here
    // so we ensure that all threads have a chance to do work
    pthread_barrier_wait(d.bar);

    while(1) {
        char *line = NULL;
        unsigned long size = 0;
        long len = 0;
        pthread_mutex_lock(d.mtx_file);
        if((len = getline(&line, &size, d.f)) == -1) {
            if(ferror(d.f)) {
                perror("Error reading line from file");
            }
            if (line != NULL) {
                free(line);
            }
            pthread_mutex_unlock(d.mtx_file);
            break;
        }
        pthread_mutex_unlock(d.mtx_file);
        int words = len > 0 ? 1 : 0;
        for(long i = 0; i < len - 2; i++) {
            if(line[i + 1] == ' ' && line[i] != ' ') {
                words++;
            }
        }

        pthread_mutex_lock(d.mtx_vars);
        *(d.lines) += 1;
        *(d.words) += words;
        *(d.chars) += len;
        pthread_mutex_unlock(d.mtx_vars);
        free(line);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Please provide one file as argument and a number\n");
        exit(1);
    }
    FILE *f = fopen(argv[1], "r");
    if(f == NULL) {
        perror("Error opening file");
        exit(1);
    }
    int n = atoi(argv[2]);
    int lines = 0, words = 0, chars = 0;
    pthread_t *th = malloc(sizeof(pthread_t) * n);
    data *args = malloc(sizeof(data));
    pthread_mutex_t mtx_file, mtx_vars;
    pthread_barrier_t bar;
    pthread_mutex_init(&mtx_file, NULL);
    pthread_mutex_init(&mtx_vars, NULL);
    pthread_barrier_init(&bar, NULL, n);
    args->f = f;
    args->mtx_file = &mtx_file;
    args->mtx_vars = &mtx_vars;
    args->bar = &bar;
    args->lines = &lines;
    args->words = &words;
    args->chars = &chars;
    for(int i = 0; i < n; i++) {
        pthread_create(&th[i], NULL, fun, args);
    }

    for(int i = 0; i < n; i++) {
        pthread_join(th[i], NULL);
    }

    free(th);
    free(args);
    printf("Lines: %d\nWords: %d\nChars: %d\n", lines, words, chars);
    fclose(f);
    return 0;
}