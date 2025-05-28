1. Write a C program that receives a filename and a number N as command-line
arguments. The main process opens the file and then creates N threads. Each thread
will read one line at a time from the file and will update some variables that hold the total
number of lines, words, and characters for the entire file processed so far.
Once all the threads terminate, the main process prints the total number of lines, words,
and characters from the given file. (The output should be equivalent to wc -lwc file).


2. Write a C program that creates two threads. The first thread generates an array of 5
integers between 0 and 1000 and prints them. After generating the array, the first thread
signals the second thread, which checks if the elements of the array are in ascending
order. If yes, it prints the array, signals the first thread to terminate, and then terminates
itself. If not, it signals the first thread to generate a new array of 5 integers. This repeats
until the first thread generates an array of 5 integers in ascending order.