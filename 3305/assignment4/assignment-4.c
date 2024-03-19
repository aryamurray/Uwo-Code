#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Process {
    int pid;  
    int arrival_time;
    int burst_time;
    int wait_time; // Added
    int turnaround_time; // Added
} Process;

void first_come_first_served(Process *processes, int count) {
    // ... implementation
}

void shortest_job_first_preemptive(Process *processes, int count) {
    // ... implementation
}

void round_robin(Process *processes, int count, int time_quantum) {
    // ... implementation
}

Process *read_csv_file(char *filename,int* num_processes) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return NULL;
    }

    // Rough estimate for how many processes you might have
    Process *processes = malloc(500 * sizeof(Process)); 

    char line[100];
    while (fgets(line, sizeof(line), fp) != NULL) {
        char *pid_str = strtok(line, ",");
        char *burst_str = strtok(NULL, ",");

        if (pid_str && burst_str) {
            processes[*num_processes].pid = atoi(pid_str + 1); // Skip 'P'
            processes[*num_processes].arrival_time = 0; // Assume all arrive at 0 
            processes[*num_processes].burst_time = atoi(burst_str);
            *num_processes++;
        }
    }

    fclose(fp);
    return processes;
}



int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        printf("Usage: %s <algorithm> <filename> [<time_quantum>]\n", argv[0]);
        return 1;
    }

    char *algorithm = argv[1];
    char *filename = argv[2];
    int time_quantum = -1;  // Initialize in case it's not used

    if (strcmp(algorithm, "-r") == 0) {
        if (argc != 4) {
            printf("Round Robin requires a time quantum.\n");
            return 1;
        }
        time_quantum = atoi(argv[3]);
    }

    // Read processes from the CSV file
    int* num_processes = 0;
    Process *processes = read_csv_file(filename, num_processes); 


    // Select appropriate scheduling algorithm
    if (strcmp(algorithm, "-f") == 0) {
        first_come_first_served(processes, num_processes);
    } else if (strcmp(algorithm, "-s") == 0) {
        shortest_job_first_preemptive(processes, num_processes);
    } else if (strcmp(algorithm, "-r") == 0) {
        round_robin(processes, num_processes, time_quantum);
    } else {
        printf("Invalid algorithm.\n");
        return 1;
    }

    calculate_statistics(processes, num_processes);

    free(processes);  // Deallocate memory
    return 0;
}