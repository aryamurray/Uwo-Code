#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_ACCOUNTS 4
#define MAX_INSTRUCTIONS_PER_LINE 10

// Structure for a transaction
typedef struct
{
    int account_id;
    int amount;
} Transaction;

typedef struct{
    char* filepath;
    int lineNum;
} Argument;

// Shared resources
int account_balances[NUM_ACCOUNTS];
pthread_mutex_t account_locks[NUM_ACCOUNTS];

// Function to process a transaction
void process_transaction(int account, int amount)
{
    pthread_mutex_lock(&account_locks[account]);

    // Critical Section
    account_balances[account] += amount;

    pthread_mutex_unlock(&account_locks[account]);
}

int parseLine(const char* input, Transaction* transactions, int* numTransactions) {
    int maxTransactions = 10; // Adjust as needed
    int numParsed = 0;

    while (*input) {
        int account_id, amount;
        if (sscanf(input, "%d %d", &account_id, &amount) == 2) {
            transactions[numParsed].account_id = account_id;
            transactions[numParsed].amount = amount;
            numParsed++;

            if (numParsed >= maxTransactions) {
                maxTransactions *= 2;
                struct Transaction* newTransactions = (Transaction*)realloc(transactions, maxTransactions * sizeof(Transaction));
                if (!newTransactions) {
                    perror("Memory reallocation failed");
                    return 0; // Return failure
                }
                transactions = newTransactions;
            }

            // Move input pointer to the next token
            while (*input && (*input != ' ')) {
                input++;
            }
            while (*input && (*input == ' ')) {
                input++;
            }
        } else {
            // Error parsing line
            fprintf(stderr, "Error parsing input: %s\n", input);
            return 0; // Return failure
        }
    }

    *numTransactions = numParsed;
    return 0;
}

// Thread function
int* thread_routine(void* arg) {
    Argument* typedArg = (Argument*)arg; // Cast the argument
    char* filename = typedArg->filepath;
    int targetLineNum = typedArg->lineNum;

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    Transaction* transactions = (struct Transaction*)malloc(10 * sizeof(Transaction));
    if (!transactions) {
        perror("Memory allocation failed");
        return EXIT_FAILURE;
    }

    char line[256];
    int currentLineNum = 1;

    while (fgets(line, sizeof(line), fp)) {
        if (currentLineNum == targetLineNum) {
            Transaction* transactions; 
            int num_instructions = 0; 

            if (parseLine(line, transactions, &num_instructions)) { 
                // Process instructions
                for (int i = 0; i < num_instructions; i++) {
                    process_transaction(transactions[i].account_id, transactions[i].amount);
                }
            } else {
                fprintf(stderr, "Error parsing line %d\n", currentLineNum);
            }

            free(transactions); // Release dynamically allocated memory
            break; 
        }
        currentLineNum++;
    }

    fclose(fp);
    free(arg); // Release memory allocated for the Argument structure
    return 0;
}

int main(int argc, char *argv[])
{
    // if (argc != 2)
    // {
    //     fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    //     return 1;
    // }

    // Initialize account balances
    FILE *fp = fopen("bankinfo.txt", "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return 1;
    }

    char line[256];
    fgets(line, sizeof(line), fp);
    char *token = strtok(line, " ");
    for (int i = 0; i < NUM_ACCOUNTS; i++)
    {
        account_balances[i] = atoi(token);
        token = strtok(NULL, " ");
    }
    fclose(fp);

    // Initialize mutexes
    for (int i = 0; i < NUM_ACCOUNTS; i++)
    {
        pthread_mutex_init(&account_locks[i], NULL);
    }


    // Print starting balances
    printf("Starting balances: ");
    for (int i = 0; i < NUM_ACCOUNTS; i++)
    {
        printf("%d ", account_balances[i]);
    }
    printf("\n");


    Transaction *instructions;
    int *num_instructions = 0;
    char testLine[255] = "2 +300 3 -400";
    parseLine(testLine,instructions,num_instructions);
    printf("Before init");
    

    // Create threads
    pthread_t threads[4];

    for (int i = 0; i < 4; i++) {
        Argument* arg = malloc(sizeof(Argument)); // Allocate memory for Argument
        if (arg == NULL) {
            fprintf(stderr, "Error allocating memory\n");
            exit(1); 
        }
        arg->filepath = "bankinfo.txt"; // Assuming a single input file
        arg->lineNum = i + 2;
        pthread_create(&threads[i], NULL, thread_routine, arg);
    }

    // Wait for threads to finish
    for (int i = 0; i < 4; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Print ending balances
    printf("Ending balances: ");
    for (int i = 0; i < NUM_ACCOUNTS; i++)
    {
        printf("%d ", account_balances[i]);
    }
    printf("\n");

    // Destroy mutexes
    for (int i = 0; i < NUM_ACCOUNTS; i++)
    {
        pthread_mutex_destroy(&account_locks[i]);
    }

    return 0;
}
