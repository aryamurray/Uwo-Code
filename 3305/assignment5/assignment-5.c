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
    int threadNum;
} Argument;

// Shared resources
int account_balances[NUM_ACCOUNTS];
pthread_mutex_t account_locks[NUM_ACCOUNTS];

// Function to process a transaction
void process_transaction(int account, int amount)
{

    while (pthread_mutex_trylock(&account_locks[account]) != 0) {
        // Lock is held, keep trying.
    }

    // Critical Section
    account_balances[account] += amount; // Adjust indexing here as well

    pthread_mutex_unlock(&account_locks[account]);
}

Transaction* parseLine(char* input) {
    int numParsed = 0;
    bool hasData = false;

    char* token = strtok(input," ");
    int counter = 0;
    int transactionPointer = 0;
    Transaction* transactions = (Transaction*)malloc(sizeof(Transaction) * MAX_INSTRUCTIONS_PER_LINE);
    Transaction* tpointer = transactions;

    while (token != 0){
        if (counter % 2 == 0){
            tpointer->account_id = atoi(token);
        }
        else{
            char firstLetter = token[0];
            char* rest = token+1;
            if (firstLetter == '+'){
                tpointer->amount = atoi(rest);
                
            }
            else if(firstLetter == '-'){
                tpointer->amount = -atoi(rest);
            }
            tpointer++;
        }
        token = strtok(NULL, " "); // Get the next token
        counter++;
    }
    return transactions;
}

// Thread function
void* thread_routine(void* arg) {
    Argument* typedArg = (Argument*)arg; // Cast the argument
    char* filename = typedArg->filepath;
    int targetLineNum = typedArg->lineNum;

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    Transaction* transactions;

    char line[256];
    int currentLineNum = 1;

    while (fgets(line, sizeof(line), fp)) {
        if (currentLineNum == targetLineNum) {


            transactions = parseLine(line);
            for (int i = 0; i < MAX_INSTRUCTIONS_PER_LINE; i++){
                if (transactions->amount == 0) continue;
                printf("Thread %i:\n",typedArg->threadNum+1);
                printf("    %s $%i %s Account %i\n",transactions->amount > 0 ? "Deposit" : "Withdraw", transactions->amount,transactions->amount > 0 ? "to" : "from",transactions->account_id);
                if (account_balances[transactions->account_id-1] + transactions->amount > 0){
                    process_transaction(transactions->account_id-1,transactions->amount);
                }
                else{
                    printf("    *** INSUFFICIENT FUNDS ***\n");
                }
                
                printf("    Account %i: $%i\n\n",transactions->account_id,account_balances[transactions->account_id-1]);
                transactions++;
                usleep(50000);
            }
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


    // Transaction *instructions;
    // int *num_instructions = 0;
    // char testLine[255] = "3 +500 4 -400 1 +100";
    // Transaction* transactions= parseLine(testLine);
    // transactions++;
    // transactions++;
    

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
        arg->threadNum = i;
        pthread_create(&threads[i], NULL, thread_routine, (void*)arg);
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
