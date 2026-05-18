# Problems

## Problem 1 — Word Count

Write a C program that receives a filename and a number N as command-line
arguments. The main process opens the file and then creates N threads. Each thread
will read one line at a time from the file and will update some variables that hold the total
number of lines, words, and characters for the entire file processed so far.
Once all the threads terminate, the main process prints the total number of lines, words,
and characters from the given file. (The output should be equivalent to `wc -lwc file`).

## Problem 2 — Sorted Array

Write a C program that creates two threads. The first thread generates an array of 5
integers between 0 and 1000 and prints them. After generating the array, the first thread
signals the second thread, which checks if the elements of the array are in ascending
order. If yes, it prints the array, signals the first thread to terminate, and then terminates
itself. If not, it signals the first thread to generate a new array of 5 integers. This repeats
until the first thread generates an array of 5 integers in ascending order.

## Problem 3 — Log Aggregator

Write a C program that receives two numbers N and M as command-line arguments.
The program creates N "worker" threads and one "logger" thread.

Each worker thread continuously generates a random integer between 1 and 100 and
appends it to a shared buffer that holds exactly 10 integers. Once the buffer is
full, the logger thread wakes up, calculates the sum and average of the 10 values,
prints them, and clears the buffer. This repeats until M buffers have been processed.

### Requirements

- Workers must not write to a full buffer — they sleep until the logger clears it.
- The logger must not act on a buffer that is not yet full — it sleeps until workers fill it.
- Once M buffers have been logged, all threads must terminate cleanly.
- Use a mutex and two condition variables.
- Do not use global variables.

### Expected output

Example with `N=3 M=2`:

```
[Buffer 1] values: 42 17 83 5 61 29 74 38 56 91  sum=496  avg=49.60
[Buffer 2] values: 13 88 7 44 32 76 19 55 62 28  sum=424  avg=42.40
```

### Compile

```
gcc -Wall -Wextra -Werror -g -o log_aggregator log_aggregator.c -pthread
```
