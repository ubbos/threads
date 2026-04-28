// POSIX Threads - section 6.2: PThread argument passing
//
// Example 1: single thread function with string argument
//   - passing a string as the void* argument
//   - reusing the same function for multiple threads
//
// Example 2: struct argument
//   - passing multiple values via a struct
//
// Example 3: race condition bug — passing &i to all threads
//   - all threads share the same pointer &i
//   - i changes in the loop, so threads print unpredictable values
//   - run multiple times to observe duplicate/missing numbers
//
// Example 4: fix 1 — array of ints, each thread gets its own element
//
// Example 5: fix 2 — malloc a separate int per thread
//
// Compile: gcc -Wall -Wextra -Werror -g -o threads_args threads_args.c -pthread

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int n = 5;

// ---- Example 1: string argument ----

void* f_string(void* a) {
    int i;
    for (i = 0; i < n; i++) {
        printf("%s\n", (char*)a);
    }
    return NULL;
}

void example1(void) {
    int i;
    pthread_t ta, tb;

    pthread_create(&ta, NULL, f_string, "fa");
    pthread_create(&tb, NULL, f_string, "fb");

    for (i = 0; i < n; i++) {
        printf("main\n");
    }

    pthread_join(ta, NULL);
    pthread_join(tb, NULL);
}

// ---- Example 2: struct argument ----

struct arg_t {
    char* name;
    int count;
};

void* f_struct(void* a) {
    int i;
    struct arg_t* x = (struct arg_t*)a;
    for (i = 0; i < x->count; i++) {
        printf("%s\n", x->name);
    }
    return NULL;
}

void example2(void) {
    int i;
    pthread_t ta, tb;
    struct arg_t aa, ab;

    aa.name = "fa"; aa.count = n;
    ab.name = "fb"; ab.count = n;

    pthread_create(&ta, NULL, f_struct, &aa);
    pthread_create(&tb, NULL, f_struct, &ab);

    for (i = 0; i < n; i++) {
        printf("main\n");
    }

    pthread_join(ta, NULL);
    pthread_join(tb, NULL);
}

// ---- Example 3: race condition bug ----

void* f_print_int(void* a) {
    printf("%d\n", *((int*)a));
    return NULL;
}

void example3_buggy(void) {
    int i;
    pthread_t t[10];

    for (i = 0; i < 10; i++) {
        pthread_create(&t[i], NULL, f_print_int, &i);  // BUG: all threads share &i
    }

    for (i = 0; i < 10; i++) {
        pthread_join(t[i], NULL);
    }
}

// ---- Example 4: fix 1 — array of ints ----

void example3_fix1(void) {
    int i, a[10];
    pthread_t t[10];

    for (i = 0; i < 10; i++) {
        a[i] = i;
        pthread_create(&t[i], NULL, f_print_int, &a[i]);
    }

    for (i = 0; i < 10; i++) {
        pthread_join(t[i], NULL);
    }
}

// ---- Example 5: fix 2 — malloc per thread ----

void* f_print_int_free(void* a) {
    printf("%d\n", *(int*)a);
    free(a);
    return NULL;
}

void example3_fix2(void) {
    int i;
    int* p;
    pthread_t t[10];

    for (i = 0; i < 10; i++) {
        p = (int*)malloc(sizeof(int));
        *p = i;
        pthread_create(&t[i], NULL, f_print_int_free, p);
    }

    for (i = 0; i < 10; i++) {
        pthread_join(t[i], NULL);
    }
}

// ---- main ----

int main(void) {
    printf("=== Example 1: string argument ===\n");
    example1();

    printf("\n=== Example 2: struct argument ===\n");
    example2();

    printf("\n=== Example 3: race condition bug (duplicate/missing numbers) ===\n");
    example3_buggy();

    printf("\n=== Example 4: fix 1 - array of ints ===\n");
    example3_fix1();

    printf("\n=== Example 5: fix 2 - malloc per thread ===\n");
    example3_fix2();

    return 0;
}
