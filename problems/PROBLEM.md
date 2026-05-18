# Log Aggregator

Write a C program that receives two numbers N and M as command-line arguments.
The program creates N "worker" threads and one "logger" thread.

Each worker thread continuously generates a random integer between 1 and 100 and
appends it to a shared buffer that holds exactly 10 integers. Once the buffer is
full, the logger thread wakes up, calculates the sum and average of the 10 values,
prints them, and clears the buffer. This repeats until M buffers have been processed.

## Requirements

- Workers must not write to a full buffer — they sleep until the logger clears it.
- The logger must not act on a buffer that is not yet full — it sleeps until workers fill it.
- Once M buffers have been logged, all threads must terminate cleanly.
- Use a mutex and two condition variables.
- Do not use global variables.

## Expected output

Example with `N=3 M=2`:

```
[Buffer 1] values: 42 17 83 5 61 29 74 38 56 91  sum=496  avg=49.60
[Buffer 2] values: 13 88 7 44 32 76 19 55 62 28  sum=424  avg=42.40
```

## Compile

```
gcc -Wall -Wextra -Werror -g -o log_aggregator log_aggregator.c -pthread
```
