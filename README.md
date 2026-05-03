# Threads — UBB Operating Systems

Examples and exercises based on the teaching notes from:
https://www.cs.ubbcluj.ro/~rares/course/os/res/teaching/teaching-notes.pdf

## Repository Structure

```
threads/
├── README.md              ← this file
│
├── threads/               ← annotated example programs (follow the teaching notes)
│   ├── README.md          ← full documentation: concepts, API reference, learning order
│   ├── 01_threads_creation.c
│   ├── 02_threads_args.c
│   ├── 03_threads_racecond.c
│   ├── 04_threads_mutex_simple.c
│   ├── 05_threads_mutex.c
│   ├── 06_threads_readwrite.c
│   ├── 07_threads_conditionals.c
│   ├── 08_threads_semaphores.c
│   ├── 09_threads_semaphores_login.c
│   ├── 10_threads_barrier.c
│   ├── 11_threads_barrier_example.c
│   ├── 12_threads_uppercase.c
│   ├── 13_threads_pass_arguments.c
│   └── 14_threads_get_arguments.c
│
└── problems/              ← practice problems (no solutions)
    └── README.md          ← problem statements
```

### `threads/`

Fourteen C programs that progressively cover POSIX thread programming. Each file corresponds to a section of the teaching notes. See [`threads/README.md`](threads/README.md) for the full learning order, concept explanations, and API reference covering:

- Thread creation and joining (`pthread_create`, `pthread_join`)
- Race conditions
- Mutexes (`pthread_mutex_t`)
- Read-write locks (`pthread_rwlock_t`)
- Condition variables (`pthread_cond_t`)
- Semaphores (`sem_t`)
- Barriers (`pthread_barrier_t`)

### `problems/`

Two practice problems in [`problems/README.md`](problems/README.md):

1. A multi-threaded `wc`-style program — threads share file reading and accumulate line/word/character counts.
2. A producer/consumer problem using condition variables — one thread generates random arrays until another thread confirms they are sorted in ascending order.

## Compile

```
gcc -Wall -Wextra -Werror -g -o a <file.c> -pthread
```