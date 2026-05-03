# POSIX Threads Examples

Examples follow the teaching notes: https://www.cs.ubbcluj.ro/~rares/course/os/res/teaching/teaching-notes.pdf

## Compile

```
gcc -Wall -Wextra -Werror -g -o a <file.c> -pthread
```

## Learning Order

| # | File | Notes Section | Topic |
|---|------|---------------|-------|
| 01 | `01_threads_creation.c` | 6.1 | Thread creation, `pthread_create`, `pthread_join`, non-deterministic scheduling |
| 02 | `02_threads_args.c` | 6.2 | Argument passing: string, struct, race condition bug and two fixes |
| 03 | `03_threads_racecond.c` | 7 | Race condition on a shared variable |
| 04 | `04_threads_mutex_simple.c` | 7.1 | Mutex basics: `pthread_mutex_lock` / `pthread_mutex_unlock` |
| 05 | `05_threads_mutex.c` | 7.1 | Mutex with struct-passed arguments |
| 06 | `06_threads_readwrite.c` | 7.2 | Read-write locks: `pthread_rwlock_rdlock` / `pthread_rwlock_wrlock` |
| 07 | `07_threads_conditionals.c` | 7.3 | Condition variables: `pthread_cond_wait` / `pthread_cond_signal` |
| 08 | `08_threads_semaphores.c` | 7.4 | Semaphore basics: `sem_wait` / `sem_post` |
| 09 | `09_threads_semaphores_login.c` | 7.4 | Semaphore example: bounded concurrent logins |
| 10 | `10_threads_barrier.c` | 7.5 | Barrier basics: `pthread_barrier_wait` |
| 11 | `11_threads_barrier_example.c` | 7.5 | Barrier example: multi-round dice game |
| 12 | `12_threads_uppercase.c` | — | Exercise: one thread per argument, uppercase conversion, return value via `pthread_join` |
| 13 | `13_threads_pass_arguments.c` | — | Exercise: passing arguments with `malloc` |
| 14 | `14_threads_get_arguments.c` | — | Exercise: getting a return value from a thread |

---

## 7.3 Condition Variables

A **condition variable** lets a thread sleep until some condition becomes true. It is always used together with a mutex.

### The problem

A mutex alone can protect shared data, but it cannot make a thread *wait* for a specific state. Without condition variables, a thread would have to busy-loop:

```c
// Bad: wastes CPU
pthread_mutex_lock(&mtx);
while (data_not_ready) {
    pthread_mutex_unlock(&mtx);
    // another thread may sneak in here and signal before we sleep — missed wakeup!
    pthread_mutex_lock(&mtx);
}
```

### Key functions

```c
#include <pthread.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
// or:
pthread_cond_init(&cond, NULL);
pthread_cond_destroy(&cond);

// Called by the waiting thread (must hold the mutex):
pthread_cond_wait(&cond, &mutex);
// Atomically: unlock mutex → sleep until signalled → re-lock mutex

// Called by the signalling thread (must hold the mutex):
pthread_cond_signal(&cond);     // wake one waiting thread
pthread_cond_broadcast(&cond);  // wake all waiting threads
```

### Correct pattern

The signal must be sent **inside** the mutex lock to prevent the missed-wakeup race. The waiting thread must re-check the condition in a `while` loop, not an `if`, to handle spurious wakeups.

```c
// Waiting thread
pthread_mutex_lock(&mtx);
while (!condition) {                    // always a while, not if
    pthread_cond_wait(&cond, &mtx);     // atomically unlocks and sleeps
}
// ... use shared data ...
pthread_mutex_unlock(&mtx);

// Signalling thread
pthread_mutex_lock(&mtx);
// ... modify shared data ...
pthread_cond_signal(&cond);             // signal BEFORE unlock
pthread_mutex_unlock(&mtx);
```

### Example — `07_threads_conditionals.c`

A `fuel_filling` thread adds 15 units of fuel five times, signalling after each fill. A `car` thread waits until at least 40 units are available, then consumes them.

---

## 7.4 Semaphores

A **semaphore** is an integer counter that controls access to a shared resource. Unlike a mutex (which is either locked or unlocked), a semaphore can allow up to *N* threads through simultaneously.

- `sem_wait` decrements the counter; if it would go below zero the thread blocks.
- `sem_post` increments the counter, waking a blocked thread if any.

A semaphore initialised to **1** behaves like a mutex. One initialised to **N** allows at most N threads to proceed concurrently.

### Key functions

```c
#include <semaphore.h>

sem_t sem;
sem_init(&sem, 0, N);   // 0 = shared between threads (not processes); N = initial count
sem_destroy(&sem);

sem_wait(&sem);          // decrement (block if 0)
sem_post(&sem);          // increment (wake one waiter)
```

> **macOS note:** `sem_init` / `sem_destroy` are deprecated on macOS. Use named semaphores (`sem_open` / `sem_close` / `sem_unlink`) instead when targeting macOS.

### Example — `08_threads_semaphores.c`

Four threads each want to print a message after sleeping for 1 second. The semaphore is initialised to **2**, so at most 2 threads run the critical section at a time, making the output appear in pairs rather than all at once.

### Example — `09_threads_semaphores_login.c`

Simulates a login system with 16 users but only **4** concurrent login slots. Each thread waits at `sem_wait` until a slot is free, "logs in" for a random time, then releases the slot with `sem_post`.

---

## 7.5 Barriers

A **barrier** makes a group of threads wait until *all* of them have reached the same point, then releases them together. This is useful when parallel work must be fully complete before the next phase begins.

### Key functions

```c
#include <pthread.h>

pthread_barrier_t barrier;
pthread_barrier_init(&barrier, NULL, count);  // count = number of threads that must arrive
pthread_barrier_destroy(&barrier);

pthread_barrier_wait(&barrier);
// Blocks until 'count' threads have called wait, then all are released simultaneously.
// Returns PTHREAD_BARRIER_SERIAL_THREAD in exactly one thread; 0 in the rest.
```

> **macOS note:** `pthread_barrier_t` is not available on macOS (it is a POSIX optional feature). These examples compile and run on Linux.

### Example — `10_threads_barrier.c`

Ten threads loop forever. Each iteration: print "Waiting at the barrier…", sleep, then call `pthread_barrier_wait`. All ten threads cross together, print "We passed the barrier", and repeat.

### Example — `11_threads_barrier_example.c`

A multi-round dice game with 8 threads and 2 barriers:

```
Phase 1 │ Each thread rolls a die → pthread_barrier_wait(barrierRolledDice)
        │ (main thread) finds the highest value, marks winners/losers
Phase 2 │ pthread_barrier_wait(barrierCalculated)
        │ Each thread prints whether it won or lost → repeat
```

Two barriers are required: the first ensures all dice are rolled before the winner is calculated; the second ensures the winner is calculated before any thread prints its result.
