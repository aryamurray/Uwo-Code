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

    printf("\n\nYour integers are %i %i\n\n", a, b);

    // Here we calculate the digits and assign them
    int a1, a2, b1, b2;
    partition(a, &a1, &a2);
    partition(b, &b1, &b2);

    // This was a quick test to ensure that the values are correct
    //  printf("%i,%i,%i,%i\n", a1, a2, b1, b2);


    // Fork a child process
    pid_t pid;
    int fd1[2]; //Used for parent to child
    int fd2[2]; //Used for child to parent
    // fd[0] - read
    // fd[1] - write
     if (pipe(fd1) != 0  || pipe(fd2) != 0){
        printf("[Error] There was an error when creating the pipes");
        return -1;
     }

    pid = fork();

    if (pid == 0)
    {
        // Child process
        int A, B, C, D, a1, a2, b1, b2;

        //PART A
        read(fd1[0], &a1, sizeof(int));
        read(fd1[0], &b1, sizeof(int));
        printf("    Child (PID %i): Received %i from parent\n", getpid(), a1);
        printf("    Child (PID %i): Received %i from parent\n", getpid(), b1);
        //Perform operation and return A
        A = a1*b1;
        printf("    Child (PID %i): Sending %i to parent\n", getpid(), A);
        write(fd2[1], &A, sizeof(int));

        //PART B
        read(fd1[0], &a2, sizeof(int));
        read(fd1[0], &b1, sizeof(int));
        printf("    Child (PID %i): Received %i from parent\n", getpid(), a2);
        printf("    Child (PID %i): Received %i from parent\n", getpid(), b1);
        //Perform operation and return A
        B = a2*b1;
        printf("    Child (PID %i): Sending %i to parent\n", getpid(), B);
        write(fd2[1], &B, sizeof(int));


        //PART C
        read(fd1[0], &a1, sizeof(int));
        read(fd1[0], &b2, sizeof(int));
        printf("    Child (PID %i): Received %i from parent\n", getpid(), a1);
        printf("    Child (PID %i): Received %i from parent\n", getpid(), b2);
        //Perform operation and return A
        C = a1*b2;
        printf("    Child (PID %i): Sending %i to parent\n", getpid(), C);
        write(fd2[1], &C, sizeof(int));


        //PART D
        read(fd1[0], &a2, sizeof(int));
        read(fd1[0], &b2, sizeof(int));
        printf("    Child (PID %i): Received %i from parent\n", getpid(), a2);
        printf("    Child (PID %i): Received %i from parent\n", getpid(), b2);
        //Perform operation and return A
        D = a2*b2;
        printf("    Child (PID %i): Sending %i to parent\n", getpid(), D);
        write(fd2[1], &D, sizeof(int));




        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
    }
    else
    {
        //PARENT PROCESS
        // Delcare the Variables
        int A, B, C, D;

        // Calculate A
        printf("###\n# Calculating X\n###\n\n");
        printf("Parent (PID %i): Sending %i to child\n", getpid(), a1);
        printf("Parent (PID %i): Sending %i to child\n", getpid(), b1);

        // Send a1 and b1 to child
        write(fd1[1], &a1, sizeof(int));
        write(fd1[1], &b1, sizeof(int));
        //Read back the value of A
        read(fd2[0], &A, sizeof(int));
        printf("Parent (PID %i): Received %i from child\n\n", getpid(), A);


        // Calculate B
        printf("###\n# Calculating Y\n###\n\n");
        printf("Parent (PID %i): Sending %i to child\n", getpid(), a2);
        printf("Parent (PID %i): Sending %i to child\n", getpid(), b1);

        // Send a2 and b1 to child
        write(fd1[1], &a2, sizeof(int));
        write(fd1[1], &b1, sizeof(int));
        //Read back the value of B
        read(fd2[0], &B, sizeof(int));
        printf("Parent (PID %i): Received %i from child\n\n", getpid(), B);

        
        // Calculate C
        printf("Parent (PID %i): Sending %i to child\n", getpid(), a1);
        printf("Parent (PID %i): Sending %i to child\n", getpid(), b2);

        // Send a1 and b2 to child
        write(fd1[1], &a1, sizeof(int));
        write(fd1[1], &b2, sizeof(int));
        //Read back the value of C
        read(fd2[0], &C, sizeof(int));
        printf("Parent (PID %i): Received %i from child\n\n", getpid(), C);


        // Calculate D
        printf("###\n# Calculating Z\n###\n\n");
        printf("Parent (PID %i): Sending %i to child\n", getpid(), a2);
        printf("Parent (PID %i): Sending %i to child\n", getpid(), b2);

        // Send a2 and b2 to child
        write(fd1[1], &a2, sizeof(int));
        write(fd1[1], &b2, sizeof(int));
        //Read back the value of A
        read(fd2[0], &D, sizeof(int));
        printf("Parent (PID %i): Received %i from child\n\n", getpid(), D);

        //Final Values
        int X,Y,Z, total;
        X = (A*pow(10,4));
        Y =  (B+C)*pow(10,2);
        Z = D;
        printf("%i*%i == %i + %i + %i == %i\n", a,b, X,Y,Z, X+Y+Z);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
    }
    return 0;
}