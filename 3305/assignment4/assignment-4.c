#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_PROCESSES 20

typedef struct
{
    int process_id;
    int arrival_time;
    int burst_time;
    int wait_time;
    int turnaround_time;
} Process;

int read_processes(const char *filename, Process processes[])
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        return -1; // Error opening file
    }

    int num_processes = 0;
    int id;
    int burstTime;

    while (fscanf(fp, "P%d,%d\n", &id, &burstTime) == 2)
    {
        processes[num_processes].process_id = id;
        processes[num_processes].burst_time = burstTime;
        processes[num_processes].wait_time = 0;
        processes[num_processes].turnaround_time = 0;
        processes[num_processes].arrival_time = id;
        num_processes++;
    }

    fclose(fp);
    return num_processes;
}

void step(Process Processes[], int num_processes, int pid)
{
    for (int i = 0; i < num_processes; i++)
    {
        if (i != pid && Processes[i].burst_time != 0)
        {
            Processes[i].wait_time++;
        }
        if (i == pid)
        {
            Processes[i].burst_time--;
        }
        if (Processes[i].burst_time != 0)
        {
            Processes[i].turnaround_time++;
        }
    }
}

void calculateStats(Process Processes[], int num_processes)
{
    int totalWaitTime = 0;
    int totalTurnAroundTime = 0;

    for (int i = 0; i < num_processes; i++)
    {
        totalWaitTime += Processes[i].wait_time;
        totalTurnAroundTime += Processes[i].turnaround_time;
    }

    printf("     Average waiting time: %.1f\n", (float)totalWaitTime / num_processes);
    printf("     Average Turnaround time: %.1f\n\n", (float)totalTurnAroundTime / num_processes);
}

// void calculateIndividualStats(Process processes, int index){

//     printf("     Average waiting time: %i\n", totalWaitTime/num_processes);
//     printf("     Average Turnaround time: %i\n\n", totalTurnAroundTime/num_processes);
// }

void simulate_fcfs(Process processes[], int num_processes)
{
    int current_time = 0;

    printf("First Come First Served\n");

    for (int i = 0; i < num_processes; i++)
    {
        processes[i].turnaround_time -= processes[i].arrival_time;
        processes[i].wait_time -= processes[i].arrival_time;

        while (processes[i].burst_time > 0)
        {
            printf("T%d : P%d - Burst left %d, Wait Time %d, Turnaround Time %d\n",
                   current_time, processes[i].process_id, processes[i].burst_time,
                   processes[i].wait_time, processes[i].turnaround_time);
            step(processes, 20, i);
            current_time++;
        }
    }

    calculateStats(processes, num_processes);
}

void simulate_sjf(Process processes[], int num_processes)
{
    int current_time = 0;

    printf("Shortest Job First\n");

    while (1)
    {
        int shortest_job_index = -1;
        int shortest_burst_time = INT_MAX;

        // Find the process with the shortest burst time
        for (int i = 0; i < num_processes; i++)
        {
            if (processes[i].arrival_time <= current_time && processes[i].burst_time < shortest_burst_time && processes[i].burst_time > 0)
            {
                shortest_burst_time = processes[i].burst_time;
                shortest_job_index = i;
            }
        }

        // If no process is found, all processes have completed
        if (shortest_job_index == -1)
        {
            break;
        }

        processes[shortest_job_index].turnaround_time -= processes[shortest_job_index].arrival_time;
        processes[shortest_job_index].wait_time -= processes[shortest_job_index].arrival_time;

        // Run the shortest job
        while (processes[shortest_job_index].burst_time > 0)
        {
            printf("T%d : P%d - Burst left %d, Wait Time %d, Turnaround Time %d\n",
                   current_time, processes[shortest_job_index].process_id, processes[shortest_job_index].burst_time,
                   processes[shortest_job_index].wait_time, processes[shortest_job_index].turnaround_time);
            step(processes, num_processes, shortest_job_index);
            current_time++;
        }
    }

    calculateStats(processes, num_processes);
}

void simulate_rr(Process processes[], int num_processes, int time_quantum)
{
    int current_time = 0;

    printf("Round Robin\n");

    int num_finished = 0;



        for (int i = 0; i < num_processes+1; i++){
            if (processes[i].burst_time > 0){
                for (int j = 0; j < time_quantum && processes[i].burst_time > 0; j++){
                    printf("T%d : P%d - Burst left %d, Wait Time %d, Turnaround Time %d\n",
                       current_time, processes[i].process_id, processes[i].burst_time,
                       processes[i].wait_time, processes[i].turnaround_time);
                    step(processes, num_processes, i);
                    current_time++;
                    if(processes[i].burst_time == 0){
                        num_finished++;
                        break;
                    }
                }
                if (num_finished == num_processes) break;
            }
            if (i == num_processes) i = 0;
        }

    calculateStats(processes, num_processes);
}


int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 4)
    {
        printf("Usage: <-flag> <conditional:time_quantum> <filename> \n\n"); // Add usage details
        return 1;
    }

    char algorithm = argv[1][1]; // 'f', 's', or 'r'
    int time_quantum = 0;
    char* filename;

    if (algorithm == 'r')
    {
        if (argc != 4) {
            printf("Usage: <-r> <time_quantum> <filename> \n\n"); // Add usage details for round robin
            return 1;
        }
        time_quantum = atoi(argv[2]);
        filename = argv[3];
    }
    else
    {
        filename = argv[2];
    }

    Process processes[MAX_PROCESSES];
    int num_processes = read_processes(filename, processes);

    // Select simulation based on algorithm
    switch (algorithm)
    {
    case 'f':
        simulate_fcfs(processes, num_processes);
        break;
    case 's':
        simulate_sjf(processes, num_processes);
        break;
    case 'r':
        simulate_rr(processes, num_processes, time_quantum);
        break;
    default:
        printf("Invalid algorithm\n");
        return 1;
    }
}
