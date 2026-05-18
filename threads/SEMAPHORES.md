# Semaphores

A **semaphore** is an integer counter that controls how many threads may access a resource simultaneously.

- `sem_wait` decrements the counter — blocks if it is already 0.
- `sem_post` increments the counter — wakes one blocked thread if any.
- Initialised to **1**: behaves like a mutex.
- Initialised to **N**: allows at most N threads through at the same time.

Unlike a mutex, `sem_post` may be called by any thread — not only the one that called `sem_wait`. This makes semaphores suitable for **signalling** as well as **limiting concurrency**.

## API

```c
#include <semaphore.h>

sem_t sem;

sem_init(&sem, 0, N);   // 0 = thread-shared (not process-shared); N = initial count
sem_wait(&sem);         // decrement; block if 0
sem_post(&sem);         // increment; wake one waiter
sem_destroy(&sem);      // free resources
```

---

## Example 1 — Limiting concurrency (`08_threads_semaphores.c`)

**Scenario:** 4 threads each sleep 1 second and print a message. Limit to 2 running simultaneously.

```c
#define THREAD_NUM 4

sem_t semaphore;

void *routine(void *args)
{
    sem_wait(&semaphore);   // acquire a slot (blocks if 2 are already inside)
    sleep(1);
    printf("Hello from thread %d\n", *(int *)args);
    sem_post(&semaphore);   // release the slot
    free(args);
    return NULL;
}

int main(void)
{
    pthread_t th[THREAD_NUM];
    sem_init(&semaphore, 0, 2);   // at most 2 threads inside at once

    for (int i = 0; i < THREAD_NUM; i++) {
        int *a = malloc(sizeof(int));
        *a = i;
        pthread_create(&th[i], NULL, &routine, a);
    }
    for (int i = 0; i < THREAD_NUM; i++)
        pthread_join(th[i], NULL);

    sem_destroy(&semaphore);
    return 0;
}
```

**What to observe:**
- Threads 0 and 1 enter immediately; threads 2 and 3 block on `sem_wait`.
- After ~1 second threads 0 and 1 call `sem_post`, releasing the two waiting threads.
- Output appears in two bursts ~1 second apart — not all at once.

**Why not a mutex here?**  
A mutex would limit concurrency to 1. A semaphore initialised to 2 allows exactly 2 — a mutex cannot express that without extra bookkeeping.

---

## Example 2 — Bounded login system (`09_threads_semaphores_login.c`)

**Scenario:** 16 users try to log in but the system allows at most 4 concurrent sessions.

```c
#define THREAD_NUM 16

sem_t semaphore;

void *routine(void *args)
{
    printf("(%d) Waiting in the login queue\n", *(int *)args);
    sem_wait(&semaphore);                   // wait for a free session slot
    printf("(%d) Logged in\n", *(int *)args);
    sleep(rand() % 5 + 1);                 // simulate work (1–5 s)
    printf("(%d) Logged out\n", *(int *)args);
    sem_post(&semaphore);                   // release the session slot
    free(args);
    return NULL;
}

int main(void)
{
    pthread_t th[THREAD_NUM];
    sem_init(&semaphore, 0, 4);             // max 4 concurrent sessions

    for (int i = 0; i < THREAD_NUM; i++) {
        int *a = malloc(sizeof(int));
        *a = i;
        pthread_create(&th[i], NULL, &routine, a);
    }
    for (int i = 0; i < THREAD_NUM; i++)
        pthread_join(th[i], NULL);

    sem_destroy(&semaphore);
    return 0;
}
```

**What to observe:**
- All 16 threads print "Waiting" almost immediately (no lock needed to print).
- Only 4 ever print "Logged in" at the same time — the semaphore enforces this.
- As a session ends (`sem_post`), the next queued thread wakes and logs in.

**Real-world analogy:** a ticket booth with 4 windows — customers queue and only 4 are served at a time.

---

## Common mistakes

| Mistake | Consequence | Fix |
|---------|-------------|-----|
| `sem_init(..., N)` where N >= thread count | Every thread passes immediately — semaphore has no effect | Choose N < thread count to actually limit concurrency |
| Missing `sem_post` on an error path | Semaphore counter never recovers — future threads block forever | Use a cleanup label or ensure every code path posts |
| Calling `sem_post` more times than `sem_wait` | Counter exceeds initial value — allows more threads than intended | Track ownership: the thread that waited should be the one that posts |

---

## When to use semaphores vs other primitives

| Need | Preferred primitive |
|------|-------------------|
| Exclusive access (1 thread at a time) | Mutex — simpler, has ownership semantics |
| Allow exactly N threads concurrently | **Semaphore** |
| Signal that data is ready (producer/consumer) | **Semaphore** (post from producer, wait in consumer) |
| Wait for a specific condition in shared data | Condition variable — more expressive |
| All threads rendezvous before proceeding | Barrier |
