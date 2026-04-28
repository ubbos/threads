// Basic POSIX thread examples following the teaching notes (chapter 6)

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// ============================================================
// Example 1 (6.1): Minimal thread creation and scheduling
//
// Demonstrates:
//   - pthread_create / pthread_join
//   - non-deterministic scheduling: "f" and "main" may print in any order
//
// Compile: gcc -Wall -Wextra -o ex1 threads_basics.c -pthread
// ============================================================

void* f_ex1(void* a) {
    (void)a;
    printf("f\n");
    return NULL;
}

void example1() {
    pthread_t t;
    pthread_create(&t, NULL, f_ex1, NULL);
    printf("main\n");
    pthread_join(t, NULL);
}

// ============================================================
// Example 2 (6.1): Two threads with iteration count
//
// Demonstrates:
//   - multiple threads sharing a global variable (read-only)
//   - interleaved scheduling visible with: ./a.out 10000 | uniq -c
// ============================================================

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

void example2() {
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

// ============================================================
// Example 3 (6.2): Single thread function with string argument
//
// Demonstrates:
//   - passing a string argument to the thread function
//   - reusing the same function for multiple threads
// ============================================================

void* f_ex3(void* a) {
    int i;
    for (i = 0; i < n; i++) {
        printf("%s\n", (char*)a);
    }
    return NULL;
}

void example3() {
    int i;
    pthread_t ta, tb;

    pthread_create(&ta, NULL, f_ex3, "fa");
    pthread_create(&tb, NULL, f_ex3, "fb");

    for (i = 0; i < n; i++) {
        printf("main\n");
    }

    pthread_join(ta, NULL);
    pthread_join(tb, NULL);
}

// ============================================================
// Example 4 (6.2): Struct argument
//
// Demonstrates:
//   - passing multiple values via a struct
// ============================================================

struct arg_t {
    char* name;
    int count;
};

void* f_ex4(void* a) {
    int i;
    struct arg_t* x = (struct arg_t*)a;
    for (i = 0; i < x->count; i++) {
        printf("%s\n", x->name);
    }
    return NULL;
}

void example4() {
    int i, count = 5;
    pthread_t ta, tb;
    struct arg_t aa, ab;

    aa.name = "fa"; aa.count = count;
    ab.name = "fb"; ab.count = count;

    pthread_create(&ta, NULL, f_ex4, &aa);
    pthread_create(&tb, NULL, f_ex4, &ab);

    for (i = 0; i < count; i++) {
        printf("main\n");
    }

    pthread_join(ta, NULL);
    pthread_join(tb, NULL);
}

// ============================================================
// Example 5 (6.2): Race condition bug — passing &i to all threads
//
// Demonstrates:
//   - all threads share the same pointer &i
//   - i changes in the loop, so threads print unpredictable values
//   - run multiple times to observe duplicate/missing numbers
// ============================================================

void* f_ex5(void* a) {
    printf("%d\n", *((int*)a));
    return NULL;
}

void example5_buggy() {
    int i;
    pthread_t t[10];

    for (i = 0; i < 10; i++) {
        pthread_create(&t[i], NULL, f_ex5, &i);  // BUG: all threads share &i
    }

    for (i = 0; i < 10; i++) {
        pthread_join(t[i], NULL);
    }
}

// ============================================================
// Example 6 (6.2): Fix 1 — array of ints, each thread gets its own element
// ============================================================

void example5_fix1() {
    int i, a[10];
    pthread_t t[10];

    for (i = 0; i < 10; i++) {
        a[i] = i;
        pthread_create(&t[i], NULL, f_ex5, &a[i]);
    }

    for (i = 0; i < 10; i++) {
        pthread_join(t[i], NULL);
    }
}

// ============================================================
// Example 7 (6.2): Fix 2 — malloc a separate int per thread
// ============================================================

void* f_ex7(void* a) {
    printf("%d\n", *(int*)a);
    free(a);
    return NULL;
}

void example5_fix2() {
    int i;
    int* p;
    pthread_t t[10];

    for (i = 0; i < 10; i++) {
        p = (int*)malloc(sizeof(int));
        *p = i;
        pthread_create(&t[i], NULL, f_ex7, p);
    }

    for (i = 0; i < 10; i++) {
        pthread_join(t[i], NULL);
    }
}

// ============================================================
// main: run all examples in sequence
// Pass an optional iteration count as argv[1] for examples 2 & 3
// ============================================================

int main(int argc, char** argv) {
    if (argc > 1) {
        sscanf(argv[1], "%d", &n);
    }

    printf("=== Example 1: minimal thread creation ===\n");
    example1();

    printf("\n=== Example 2: two threads, iteration count (pipe through 'uniq -c' to see scheduling) ===\n");
    example2();

    printf("\n=== Example 3: single function, string argument ===\n");
    example3();

    printf("\n=== Example 4: struct argument ===\n");
    example4();

    printf("\n=== Example 5: race condition bug (duplicate/missing numbers) ===\n");
    example5_buggy();

    printf("\n=== Example 6: fix 1 - array of ints ===\n");
    example5_fix1();

    printf("\n=== Example 7: fix 2 - malloc per thread ===\n");
    example5_fix2();

    return 0;
}
