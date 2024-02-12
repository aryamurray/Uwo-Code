#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>


void partition(int number, int* part1, int* part2) {
    *part1 = number / 100;
    *part2 = number % 100;
}


int main(int argc, char* argv[]){

    if (argc != 3){
        printf("[Error] Expected Usage requires two integer arguements");
        return -1;
    }
    //TODO: Handle Error Checking for integer Length

    int a = atoi(argv[1]);
    int b = atoi(argv[2]);

    printf("Your Integers are %i %i",a, b);

    int a1, a2, b1, b2;
    partition(a, &a1, &a2);
    partition(b, &b1, &b2);


    return 0;
}