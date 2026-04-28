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
