// POSIX Threads - section 6.1: Pthread creation and scheduling
//
// Example 1: minimal thread creation
//   - pthread_create / pthread_join
//   - non-deterministic scheduling: "f" and "main" may print in any order
//
// Example 2: two threads with iteration count
//   - multiple threads sharing a global variable (read-only)
//   - interleaved scheduling visible with: ./threads_creation 10000 | uniq -c
//
// Compile: gcc -Wall -Wextra -Werror -g -o threads_creation threads_creation.c -pthread

#include <stdio.h>
#include <pthread.h>

// ---- Example 1 ----

void* f(void* a) {
    (void)a;
    printf("f\n");
    return NULL;
}

void example1(void) {
    pthread_t t;
    pthread_create(&t, NULL, f, NULL);
    printf("main\n");
    pthread_join(t, NULL);
}

// ---- Example 2 ----

int n = 1;

void* fa(void* a) {
    (void)a;
    int i;
    for (i = 0; i < n; i++) {
        printf("fa\n");
    }
    return NULL;
}

void* fb(void* a) {
    (void)a;
    int i;
    for (i = 0; i < n; i++) {
        printf("fb\n");
    }
    return NULL;
}

void example2(void) {
    int i;
    pthread_t ta, tb;

    pthread_create(&ta, NULL, fa, NULL);
    pthread_create(&tb, NULL, fb, NULL);

    for (i = 0; i < n; i++) {
        printf("main\n");
    }

    pthread_join(ta, NULL);
    pthread_join(tb, NULL);
}

// ---- main ----
// Pass an optional iteration count as argv[1] for example 2
// e.g.: ./threads_creation 10000 | uniq -c

int main(int argc, char** argv) {
    if (argc > 1) {
        sscanf(argv[1], "%d", &n);
    }

    printf("=== Example 1: minimal thread creation ===\n");
    example1();

    printf("\n=== Example 2: two threads, iteration count ===\n");
    example2();

    return 0;
}
