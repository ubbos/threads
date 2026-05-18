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

## Choosing a synchronization primitive

| Primitive | Use when... | Key property |
|-----------|-------------|--------------|
| **Mutex** | Any shared write — you need exclusive access | Binary lock: one thread at a time |
| **Read-Write Lock** | Reads are frequent, writes are rare | Many concurrent readers OR one exclusive writer |
| **Condition Variable** | A thread must wait for a *specific state* in shared data | Sleep/wake based on a condition; always paired with a mutex |
| **Semaphore** | Limit N concurrent accesses, or signal across threads | Counter: allows up to N threads through simultaneously |
| **Barrier** | All threads must finish phase N before any starts phase N+1 | Rendezvous point — releases all at once |

If you are unsure, start with a mutex. Upgrade to a rwlock when profiling shows reader contention; introduce condition variables when you find yourself polling a flag inside a locked loop.

---

## 7.1 Mutexes

A **mutex** (mutual exclusion lock) ensures that only one thread at a time can execute a critical section — a block of code that reads or writes shared data. All other threads that try to lock the mutex block until the owning thread unlocks it.

**When to use:** any time two or more threads access the same variable and at least one of them writes to it.

### Operations

| Operation | Function | Description |
|-----------|----------|-------------|
| Initialise | `pthread_mutex_init(&mtx, NULL)` | Set up the mutex before use. Can also use the static initialiser `PTHREAD_MUTEX_INITIALIZER`. |
| Lock | `pthread_mutex_lock(&mtx)` | Acquire the mutex. Blocks if another thread holds it. |
| Unlock | `pthread_mutex_unlock(&mtx)` | Release the mutex, allowing one waiting thread to proceed. |
| Destroy | `pthread_mutex_destroy(&mtx)` | Free resources after the mutex is no longer needed. |

```c
#include <pthread.h>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
// or: pthread_mutex_init(&mtx, NULL);

pthread_mutex_lock(&mtx);
// --- critical section ---
shared_variable++;
// ------------------------
pthread_mutex_unlock(&mtx);

pthread_mutex_destroy(&mtx);
```

### Practical examples

| Scenario | Why a mutex fits |
|----------|-----------------|
| Bank account balance updated by multiple threads | Only one transfer at a time must modify the balance |
| Log file written by many threads | Lines must not interleave mid-write |
| Reference counter (e.g. shared pointer) | Increment/decrement must be atomic |

**In this repo:**

- `04_threads_mutex_simple.c` — a shared `mails` counter is incremented by 4 threads 10,000,000 times each. Without the mutex the final value is wrong due to lost updates; with it the result is always 40,000,000.
- `05_threads_mutex.c` — the mutex is bundled inside a `data` struct and passed to 1,000 threads, each incrementing a counter 1,000 times. Expected final value: 1,000,000.

---

## 7.2 Read-Write Locks

A **read-write lock** distinguishes between readers and writers. Multiple threads may read simultaneously (shared access), but a writer requires exclusive access — no other readers or writers can proceed while a write is in progress.

**When to use:** when reads are frequent and writes are rare. A plain mutex would serialise all readers unnecessarily.

| Access type | Concurrency |
|-------------|-------------|
| Multiple readers | Allowed simultaneously |
| One writer | Exclusive — blocks all readers and other writers |

### Operations

| Operation | Function | Description |
|-----------|----------|-------------|
| Initialise | `pthread_rwlock_init(&rw, NULL)` | Set up the lock before use. |
| Read lock | `pthread_rwlock_rdlock(&rw)` | Acquire shared (read) access. Blocks only if a writer holds the lock. |
| Write lock | `pthread_rwlock_wrlock(&rw)` | Acquire exclusive (write) access. Blocks until all readers and writers release. |
| Unlock | `pthread_rwlock_unlock(&rw)` | Release either a read or write lock. |
| Destroy | `pthread_rwlock_destroy(&rw)` | Free resources. |

```c
#include <pthread.h>

pthread_rwlock_t rw;
pthread_rwlock_init(&rw, NULL);

// Reader thread
pthread_rwlock_rdlock(&rw);
printf("%d\n", shared_value);   // safe to read concurrently
pthread_rwlock_unlock(&rw);

// Writer thread
pthread_rwlock_wrlock(&rw);
shared_value++;                 // exclusive access
pthread_rwlock_unlock(&rw);

pthread_rwlock_destroy(&rw);
```

### Practical examples

| Scenario | Why a read-write lock fits |
|----------|---------------------------|
| In-memory cache read by many threads, refreshed occasionally | Many readers can proceed in parallel; only a cache refresh needs exclusive access |
| Configuration object loaded once, read everywhere | All threads read freely; the rare config reload takes exclusive access |
| Shared leaderboard (frequent reads, infrequent score updates) | Readers see consistent scores; a writer locks only during the update |

**In this repo:**

- `06_threads_readwrite.c` — 10 reader threads each print the current value of a counter 10 times; 2 writer threads each increment it 1,000 times. Readers never block each other — multiple `printf` calls run in parallel — but each writer gets exclusive access for its increment.

---

## 7.3 Condition Variables

A **condition variable** lets a thread sleep until some condition becomes true. It is always used together with a mutex.

**When to use:** when a thread needs to wait for a specific state in shared data (e.g. "wait until the queue is non-empty"), not just for exclusive access.

### Operations

| Operation | Function | Description |
|-----------|----------|-------------|
| Initialise | `pthread_cond_init(&cond, NULL)` | Set up the condition variable. Can also use `PTHREAD_COND_INITIALIZER`. |
| Wait | `pthread_cond_wait(&cond, &mtx)` | Atomically unlock the mutex and sleep. Re-locks the mutex before returning. Must be called with the mutex held. |
| Signal | `pthread_cond_signal(&cond)` | Wake one waiting thread. |
| Broadcast | `pthread_cond_broadcast(&cond)` | Wake all waiting threads. |
| Destroy | `pthread_cond_destroy(&cond)` | Free resources. |

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

### Practical examples

| Scenario | Why a condition variable fits |
|----------|------------------------------|
| Producer/consumer queue | Consumer sleeps until the producer adds an item; producer signals |
| Thread pool worker waiting for a task | Worker sleeps on an empty queue; dispatcher signals when a job arrives |
| Car waiting for enough fuel | Car thread sleeps until fuel level reaches the required amount |

**In this repo:**

- `07_threads_conditionals.c` — a `fuel_filling` thread adds 15 units of fuel five times, signalling the condition variable after each fill (while still holding the mutex). A `car` thread waits in a `while` loop until at least 40 units are available, then consumes them and exits.

**From Horea's problem set:**

| Problem | How condition variables are used |
|---------|----------------------------------|
| 17 | Two threads alternate appending even/odd numbers to an array. Each thread signals the other after appending, and waits if it is not its turn. |
| 27 | N generator threads fill a 128-character buffer; a printer thread waits on a condition until the buffer is full, then prints and clears it. |
| 28 | N sorter threads randomly swap array elements; the (N+1)th thread waits on a condition until a sorter detects the array is sorted, then prints it. |
| 30 | Relay race — each thread in a team of 4 waits for the previous thread to signal before it starts its leg. |

### Common mistakes

| Mistake | Consequence | Fix |
|---------|-------------|-----|
| Using `if` instead of `while` around `pthread_cond_wait` | Spurious wakeups cause the thread to proceed when the condition is still false | Always use `while (!condition)` |
| Calling `pthread_cond_signal` outside the mutex | Signal can fire between the check and the sleep — missed wakeup | Call signal while holding the mutex |
| Forgetting that `pthread_cond_wait` re-acquires the mutex on return | Double-lock or assuming the condition cannot change after waking | Re-check the condition in the `while` loop |

---

## 7.4 Semaphores

A **semaphore** is an integer counter that controls access to a shared resource. Unlike a mutex (which is either locked or unlocked), a semaphore can allow up to *N* threads through simultaneously.

**When to use:** when you need to limit the number of threads concurrently accessing a resource (e.g. at most 4 database connections, at most 2 threads in a critical section). Also useful for signalling between threads, since any thread may call `sem_post` — not just the one that called `sem_wait`.

- A semaphore initialised to **1** behaves like a mutex.
- A semaphore initialised to **N** allows at most N threads to proceed concurrently.

### Operations

| Operation | Function | Description |
|-----------|----------|-------------|
| Initialise | `sem_init(&sem, 0, N)` | Set the counter to N. Second argument `0` means shared between threads (not processes). |
| Wait (down) | `sem_wait(&sem)` | Decrement the counter. Blocks if the counter is 0. |
| Post (up) | `sem_post(&sem)` | Increment the counter. Wakes one blocked thread if any. |
| Destroy | `sem_destroy(&sem)` | Free resources. |

### Key functions

```c
#include <semaphore.h>

sem_t sem;
sem_init(&sem, 0, N);   // 0 = shared between threads (not processes); N = initial count
sem_destroy(&sem);

sem_wait(&sem);          // decrement (block if 0)
sem_post(&sem);          // increment (wake one waiter)
```

### Practical examples

| Scenario | Why a semaphore fits |
|----------|---------------------|
| Database connection pool (max 10 connections) | `sem_init(..., 10)` — each thread acquires a slot before querying, releases after |
| Rate-limiting API calls (max 3 concurrent requests) | `sem_init(..., 3)` — excess threads block until a request finishes |
| Login system with limited concurrent sessions | `sem_init(..., N)` — only N users can be logged in at once |
| Signalling between a producer and consumer | Producer calls `sem_post` after adding an item; consumer calls `sem_wait` before taking one |

**In this repo:**

- `08_threads_semaphores.c` — 4 threads each sleep 1 second then print a message. With `sem_init(..., 2)` only 2 threads enter the critical section at a time, so output appears in pairs ~1 second apart instead of all at once.
- `09_threads_semaphores_login.c` — 16 user threads simulate a login system with only 4 concurrent slots (`sem_init(..., 4)`). Each thread prints "Waiting", acquires the semaphore, prints "Logged in", sleeps a random amount, prints "Logged out", then releases. At most 4 are ever logged in simultaneously.

**From Horea's problem set:**

| Problem | How semaphores are used |
|---------|------------------------|
| 20 | M checkpoints, each allowing only 1 thread at a time. Each checkpoint is a `sem_init(..., 1)`. All threads wait on a start semaphore until all have been created. |
| 21 | Checkpoints with halving capacity: checkpoint X allows `2^(N-1-X)` threads simultaneously. Each is a semaphore initialised to that value. |

### Common mistakes

| Mistake | Consequence | Fix |
|---------|-------------|-----|
| Initialising to the wrong N | Too large: resource is never actually limited. Too small: deadlock. | Think: "how many threads should be inside at the same time?" |
| Calling `sem_post` from the wrong thread | Releases a slot that was never acquired — N can exceed the intended limit | Ensure every `sem_wait` has exactly one matching `sem_post` per thread |

---

## 7.5 Barriers

A **barrier** makes a group of threads wait until *all* of them have reached the same point, then releases them together. This is useful when parallel work must be fully complete before the next phase begins.

**When to use:** multi-phase parallel algorithms where phase N+1 must not start until every thread has finished phase N.

### Operations

| Operation | Function | Description |
|-----------|----------|-------------|
| Initialise | `pthread_barrier_init(&b, NULL, N)` | Set the threshold to N threads. |
| Wait | `pthread_barrier_wait(&b)` | Block until N threads have called wait. All are released simultaneously. Returns `PTHREAD_BARRIER_SERIAL_THREAD` in exactly one thread, `0` in the rest. |
| Destroy | `pthread_barrier_destroy(&b)` | Free resources. |

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

### Practical examples

| Scenario | Why a barrier fits |
|----------|--------------------|
| Parallel matrix multiplication — each thread computes one row | All rows must be done before the result matrix is used |
| Multi-threaded simulation with discrete time steps | All threads must finish step N before any thread begins step N+1 |
| Parallel sort — split, sort, then merge | Sorting phase must complete in all threads before the merge begins |
| Multi-round game where all players must act before results are shown | All players roll/move, then results are revealed simultaneously |

**In this repo:**

- `10_threads_barrier.c` — 10 threads loop indefinitely. Each iteration every thread prints "Waiting at the barrier…" and sleeps, then calls `pthread_barrier_wait`. Only when all 10 have arrived do they all print "We passed the barrier" and start the next round together.
- `11_threads_barrier_example.c` — a multi-round dice game with 8 player threads and a judge (main thread), using two barriers per round:
  - **Barrier 1** (`barrierRolledDice`) — all 8 threads roll their dice; the judge waits here too. Once everyone has rolled, the judge calculates the winner.
  - **Barrier 2** (`barrierCalculated`) — threads wait while the judge works. Once the judge posts results, all threads cross together and print whether they won or lost.

**From Horea's problem set:**

| Problem | How barriers are used |
|---------|-----------------------|
| 19 | Tree-sum: threads at level N+1 must all finish before level N threads can add their results. A barrier separates each level of the tree. |
| 28 | The barrier solution (`28-barr.c`) uses a barrier so all sorter threads synchronize between swap rounds before the checker thread inspects the array. |
| 32 | After each reading pass (1–3 words per thread), all threads wait at a barrier before starting the next pass, ensuring no thread races ahead. |

### Common mistakes

| Mistake | Consequence | Fix |
|---------|-------------|-----|
| Initialising with the wrong thread count | Too few: barrier releases early. Too many: deadlock — barrier never triggers. | Count must equal exactly the number of threads that will call `pthread_barrier_wait` |
| Reusing a barrier while some threads have not yet crossed | Undefined behaviour — late threads may enter the next round's barrier | Ensure all threads cross before any thread re-enters the barrier |
