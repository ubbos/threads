/* Create a C program that receives any number of command line arguments.

 * For each argument, the program will create a thread.

 * Each thread converts any lowercase letters into uppercase letters and prints the initial string and the result.

 */


 gcc -Wall -Wextra -Werror -g -o a a.c -pthread

 or

 gcc -Wall -Wextra -Werror -g -o a a.c -lpthread



 For more documentation reach out for the teaching notes https://www.cs.ubbcluj.ro/~rares/course/os/res/teaching/teaching-notes.pdf


 6.1 PTHREAD CREATION AND SCHEDULING
 7   POSIX SYNCHRONIZATION MECHANISMS
 7.1 MUTEXES
 7.2 READ-WRITE LOCKS