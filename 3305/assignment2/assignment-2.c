#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>

void partition(int number, int *part1, int *part2)
{
    // This function returns the proper parts of the given integers
    *part1 = number / 100;
    *part2 = number % 100;
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("[Error] Expected Usage requires two integer arguements\n");
        return -1;
    }
    // TODO: Handle Error Checking for integer Length

    int a = atoi(argv[1]);
    int b = atoi(argv[2]);

    printf("Your integers are %i %i\n", a, b);

    // Here we calculate the digits and assign them
    int a1, a2, b1, b2;
    partition(a, &a1, &a2);
    partition(b, &b1, &b2);

    // This was a quick test to ensure that the values are correct
    //  printf("%i,%i,%i,%i\n", a1, a2, b1, b2);

    return 0;

    // Fork a child process
    pid_t pid;
    int fd[2];
    // fd[0] - read
    // fd[1] - write
    pipe(fd);

    pid = fork();

    if (pid == 0)
    {
        // Child process
        close(fd[1]); // Close write end of the pipe
        int A, a1, b1;
        read(fd[0], &a1, sizeof(int));
        printf(" Child (PID %i): Received %i from parent", getpid(), a1);
        read(fd[0], &b1, sizeof(int));
        printf(" Child (PID %i): Received %i from parent", getpid(), b1);
        close(fd[0]);
    }
    else
    {
        // Delcare the Variables
        int A, B, C, D;
        // Parent process
        close(fd[0]); // Close read end of the pipe

        printf("###\
                # Calculating X\
                ###");
        printf("Parent (PID %i): Sending %i to child", getpid(), a1);
        printf("Parent (PID %i): Sending %i to child", getpid(), a2);

        // Send a1 and b1 to child
        write(fd[1], &a1, sizeof(int));
        write(fd[1], &b1, sizeof(int));
        close(fd[1]);

        read(fd[0], &A, sizeof(int));
        printf("%i", A);
    }
}