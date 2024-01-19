#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define NUMFORKS 4

bool isPrime(int num){
    if (num < 2) return false;
    if (num == 2) return true;
    if (num % 2 == 0) return false;
    for (int i = 3; i <= sqrt(num); i += 2){
        if (num % i == 0) return false;
    }
    return true;
}

long sumPrimes(int beginnning, int ending){
    long sum = 0;
    int count = 0;
    for (int i = beginnning; i < ending; i++){
        if (isPrime(i)){
            sum += i;
            count++;
        }
    }
    printf("pid: %d, ppid %d - Count and sum of prime numbers between %d and %d are %d and %ld\n", getpid(), getppid(), beginnning, ending, count, sum);
    return sum;
}

int main(int argc, char* argv[]){
    //Insuficient number of arguements
    if (argc < 4){
        return -1;
    }
    //Set Variables via CLI
    int mode;
    int min = atoi(argv[2]);
    int max = atoi(argv[3]);
    int range = (max-min)/NUMFORKS;

    //Parallel
    if (*argv[1] == '0'){
        printf("Conducting Parallel Execution\n\n");
        pid_t pids[4];
        for(int i = 0; i < NUMFORKS; i++) {
            pids[i] = fork();
            if(pids[i] == 0) {
                sumPrimes(min + i*range, min + (i+1)*range);
                exit(0);
            } else if (pids[i] < 0) {
                perror("fork");
                return -1;
            }
        }
        for(int i = 0; i < 4; i++) {
            waitpid(pids[i], NULL, 0);
        }
    }
    //Serial
    else{
        printf("Conducting Serial Execution\n\n");
        sumPrimes(min,range);
        sumPrimes(range,range*2);
        sumPrimes(range*2,range*3);
        sumPrimes(range*3,max);
    }


    return 1;
}