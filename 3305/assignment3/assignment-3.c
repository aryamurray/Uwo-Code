#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define PUZZLE_SIZE 9
#define NUM_THREADS 27

int puzzle[PUZZLE_SIZE][PUZZLE_SIZE];
bool thread_results[NUM_THREADS];

void *check_row(void *arg);
void *check_column(void *arg);
void *check_subgrid(void *arg);
bool read_puzzle(char *filename);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: assignment-3 <filename>\n");
        return 1;
    }

    if (!read_puzzle(argv[1]))
    {
        printf("Error reading puzzle file.\n");
        return 1;
    }

    pthread_t threads[NUM_THREADS];
    int thread_index = 0;

    // Create threads
    for (int i = 0; i < PUZZLE_SIZE; i++)
    {
        pthread_create(&threads[thread_index++], NULL, check_row, (void *)i);
        pthread_create(&threads[thread_index++], NULL, check_column, (void *)i);
    }
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            pthread_create(&threads[thread_index++], NULL, check_subgrid, (void *)(i * 3 + j));
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Check overall result
    bool is_valid = true;
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (!thread_results[i])
        {
            is_valid = false;
        }

        if (i < 9)
        {
            printf("Thread #%2d (row %d) is %s\n", i + 1, i + 1, thread_results[i] ? "valid" : "invalid");
        }
        else if (i < 18)
        {
            printf("Thread #%2d (column %d) is %s\n", i + 1, i - 8, thread_results[i] ? "valid" : "invalid");
        }
        else
        {
            printf("Thread #%2d (subgrid %d) is %s\n", i + 1, i - 17, thread_results[i] ? "valid" : "invalid");
        }
    }

    if (is_valid)
    {
        printf("Valid Sudoku solution!\n");
    }
    else
    {
        printf("Invalid Sudoku solution.\n");
    }

    return 0;
}

void *check_row(void *arg)
{
    int row_index = (int)arg;
    bool seen[10] = {false};

    for (int col = 0; col < PUZZLE_SIZE; col++)
    {
        int digit = puzzle[row_index][col];
        if (seen[digit])
        {
            thread_results[row_index] = false;
            pthread_exit(NULL);
        }
        seen[digit] = true;
    }

    thread_results[row_index] = true;
    pthread_exit(NULL);
}

void *check_column(void *arg)
{
    int col_index = (int)arg;
    bool seen[10] = {false};

    for (int row = 0; row < PUZZLE_SIZE; row++)
    {
        int digit = puzzle[row][col_index];
        if (seen[digit])
        {
            thread_results[9 + col_index] = false;
            pthread_exit(NULL);
        }
        seen[digit] = true;
    }

    thread_results[9 + col_index] = true;
    pthread_exit(NULL);
}

void *check_subgrid(void *arg)
{
    int subgrid_index = (int)arg;
    bool seen[10] = {false};

    int row_start = (subgrid_index / 3) * 3;
    int col_start = (subgrid_index % 3) * 3;

    for (int row = row_start; row < row_start + 3; row++)
    {
        for (int col = col_start; col < col_start + 3; col++)
        {
            int digit = puzzle[row][col];
            if (seen[digit])
            {
                thread_results[18 + subgrid_index] = false;
                pthread_exit(NULL);
            }
            seen[digit] = true;
        }
    }

    thread_results[18 + subgrid_index] = true;
    pthread_exit(NULL);
}

bool read_puzzle(char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        return false; // File opening error
    }

    for (int i = 0; i < PUZZLE_SIZE; i++)
    {
        for (int j = 0; j < PUZZLE_SIZE; j++)
        {
            if (fscanf(fp, "%d", &puzzle[i][j]) != 1)
            {
                fclose(fp);
                return false; // Read error or invalid format
            }
        }
    }

    fclose(fp);
    return true;
}
