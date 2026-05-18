# Barriers

A **barrier** is a synchronization point that blocks every thread that reaches it until a specified number of threads have all arrived. Once the last thread arrives, all are released simultaneously.

Use a barrier when parallel work is split into **phases** and phase N+1 must not begin until every thread has completed phase N.

## API

```c
#include <pthread.h>

pthread_barrier_t barrier;

pthread_barrier_init(&barrier, NULL, N);  // N = number of threads that must arrive
pthread_barrier_wait(&barrier);           // block until N threads have called wait
                                          // returns PTHREAD_BARRIER_SERIAL_THREAD
                                          // in exactly one thread; 0 in the rest
pthread_barrier_destroy(&barrier);
```

The one thread that receives `PTHREAD_BARRIER_SERIAL_THREAD` can be used to perform a single post-phase action (e.g. printing results, checking a condition) before all threads continue.

---

## Example 1 — Basic barrier (`10_threads_barrier.c`)

**Scenario:** 10 threads loop forever. Each round every thread must arrive at the barrier before any of them continues to the next round.

```c
pthread_barrier_t barrier;

void *routine(void *args)
{
    (void)args;
    while (1) {
        printf("Waiting at the barrier...\n");
        sleep(1);
        pthread_barrier_wait(&barrier);   // block until all 10 threads arrive
        printf("We passed the barrier\n");
        sleep(1);
    }
}

int main(void)
{
    pthread_t th[10];
    pthread_barrier_init(&barrier, NULL, 10);   // threshold = 10

    for (int i = 0; i < 10; i++)
        pthread_create(&th[i], NULL, &routine, NULL);

    for (int i = 0; i < 10; i++)
        pthread_join(th[i], NULL);

    pthread_barrier_destroy(&barrier);
    return 0;
}
```

**What to observe:**
- All 10 "Waiting…" messages appear (in any order) within the first second.
- None of the "We passed the barrier" messages appear until all 10 have slept and called `pthread_barrier_wait`.
- Then all 10 "We passed the barrier" messages appear together, and the cycle repeats.

**Without a barrier:** a fast thread would race ahead to round 2 while slow threads are still in round 1 — rounds become unsynchronized.

---

## Example 2 — Multi-round dice game (`11_threads_barrier_example.c`)

**Scenario:** 8 player threads and 1 judge (main thread) play an infinite dice game. Each round has two phases separated by two barriers.

```
Round N:
  Phase 1 — all 8 threads roll their dice
            ---- barrierRolledDice (threshold 9 = 8 threads + main) ----
  Phase 2 — main thread calculates the winner; threads wait
            ---- barrierCalculated (threshold 9) ----
  Phase 3 — all threads print "I won" or "I lost" and start round N+1
```

```c
#define THREAD_NUM 8

int dice_values[THREAD_NUM];
int status[THREAD_NUM];   // 1 = winner, 0 = loser

pthread_barrier_t barrierRolledDice;
pthread_barrier_t barrierCalculated;

void *roll(void *args)
{
    int index = *(int *)args;
    free(args);
    while (1) {
        dice_values[index] = rand() % 32 + 1;          // roll
        pthread_barrier_wait(&barrierRolledDice);       // wait for all to roll

        pthread_barrier_wait(&barrierCalculated);       // wait for judge to score

        if (status[index] == 1)
            printf("(%d rolled %d) I won\n", index, dice_values[index]);
        else
            printf("(%d rolled %d) I lost\n", index, dice_values[index]);
    }
    return NULL;
}

int main(void)
{
    srand(time(NULL));
    pthread_t th[THREAD_NUM];

    // threshold = THREAD_NUM + 1: 8 players + main must all arrive
    pthread_barrier_init(&barrierRolledDice, NULL, THREAD_NUM + 1);
    pthread_barrier_init(&barrierCalculated, NULL, THREAD_NUM + 1);

    for (int i = 0; i < THREAD_NUM; i++) {
        int *a = malloc(sizeof(int));
        *a = i;
        pthread_create(&th[i], NULL, &roll, a);
    }

    while (1) {
        pthread_barrier_wait(&barrierRolledDice);   // wait for all rolls

        // Find the highest value
        int max = 0;
        for (int i = 0; i < THREAD_NUM; i++)
            if (dice_values[i] > max) max = dice_values[i];

        // Mark winners
        for (int i = 0; i < THREAD_NUM; i++)
            status[i] = (dice_values[i] == max) ? 1 : 0;

        sleep(1);
        printf("==== New round starting ====\n");
        pthread_barrier_wait(&barrierCalculated);   // release players
    }
}
```

**Key design points:**

| Detail | Why |
|--------|-----|
| Both barriers initialised to `THREAD_NUM + 1` | Main thread participates in both barriers — it must be counted |
| Two separate barriers | One barrier is not enough: players must be held while the judge scores, and the judge must be held while players roll |
| `status[]` written only after `barrierRolledDice` | All `dice_values[]` are final by then — no data race |
| `dice_values[]` read only after `barrierRolledDice` | Same guarantee for the judge |

---

## Common mistakes

| Mistake | Consequence | Fix |
|---------|-------------|-----|
| Wrong threshold (too small) | Barrier releases before all threads arrive — some threads skip ahead | Count every thread that calls `pthread_barrier_wait`, including main if it participates |
| Wrong threshold (too large) | Barrier never triggers — all threads block forever (deadlock) | Same fix |
| Reusing a barrier before all threads have crossed | Late threads from round N enter round N+1's barrier — count is wrong | Ensure all threads cross before any loop back; two alternating barriers can help |

---

## When to use barriers vs other primitives

| Need | Preferred primitive |
|------|-------------------|
| Exclusive access to shared data | Mutex |
| Wait for a specific condition | Condition variable |
| Limit N concurrent accesses | Semaphore |
| All threads finish phase N before any starts phase N+1 | **Barrier** |
| One thread signals another that data is ready | Condition variable or semaphore |
