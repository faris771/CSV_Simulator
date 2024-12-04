#include "header.h"
#include "shared_memories.h"
#include "message_queues.h"
#include "semphores.h"
#include "constants.h"
#include "functions.h"

// Declare global variables
sem_t *sem;                  // Semaphore for synchronizing access to shared memory
int shm_id;                  // Shared memory ID
struct shared_memory *shm_ptr; // Pointer to shared memory
int msg_queue_id;            // Message queue ID

void process_csv_file(); // Function to process CSV file and calculate sums and averages

int main(int argc, char **argv) {
    printf("File calculator Running...\n");



    // Initialize shared memory, semaphore, and message queue
    sem = setup_semaphore();
    shm_id = setup_shared_memory();
    shm_ptr = attach_shared_memory(shm_id);
    msg_queue_id = setup_message_queue();

    // Process files continuously from the message queue
    process_csv_file();

    // Cleanup resources
    detach_shared_memory(shm_ptr);
    cleanup_semaphore();
    cleanup_message_queue();

    return 0;
}

void process_csv_file() {
    char file_path[MSG_SIZE]; // Variable to hold the file path

    while (1) {
        // Wait for the file path from the message queue
        receive_message(msg_queue_id, 1, file_path); // Only process type-1 messages

        // Extract file serial number from the filename
        char *filename = strrchr(file_path, '/');
        if (filename == NULL) continue;
        filename++; // Skip the '/' character

        int fileSerial;
        if (sscanf(filename, "%d.csv", &fileSerial) != 1) continue;

        // Open the CSV file
        FILE *file = fopen(file_path, "r");
        if (!file) {
            perror("CSV_calculator_________Error opening CSV file");
            continue;
        }

        // Processing logic (e.g., calculating averages)
        int rows = 0, cols = 0;
        double columnSums[MAX_COLUMNS] = {0};
        long long columnCounts[MAX_COLUMNS] = {0};
        char line[1024];

        // Get column count from the first line
        if (fgets(line, sizeof(line), file) != NULL) {
            char *token = strtok(line, ",");
            while (token != NULL) {
                cols++;
                token = strtok(NULL, ",");
            }
        }

        // Process rows and calculate sums and averages
        while (fgets(line, sizeof(line), file) != NULL) {
            rows++;
            char *token = strtok(line, ",");
            int colIdx = 0;

            while (token != NULL && colIdx < cols) {
                if (strcmp(token, " ") != 0 && strcmp(token, "") != 0) {
                    double value = atof(token);
                    columnSums[colIdx] += value;
                    columnCounts[colIdx]++;
                }
                token = strtok(NULL, ",");
                colIdx++;
            }
        }

        fclose(file);

        // Update shared memory (synchronized with semaphore)
        semaphore_wait(sem);
        shm_ptr->numRows[fileSerial] = rows;

        // Print computed averages for verification
        printf("File %d.csv processed:\n", fileSerial);
        for (int i = 0; i < cols; i++) {
            double average = (columnCounts[i] > 0) ? columnSums[i] / columnCounts[i] : 0;
            shm_ptr->columnAverages[fileSerial][i] = average; // Update shared memory
            printf("  Column %d: Sum = %.2f, Count = %lld, Average = %.2f\n",
                   i + 1, columnSums[i], columnCounts[i], average);

            // Update min and max averages
            if (average < shm_ptr->minAvg) {
                printf("======================================================\n");
                printf("Min average updated:from %.2f to %.2f\n", shm_ptr->minAvg, average);

                shm_ptr->minAvg = average;
                shm_ptr->minAvgColumn = i;
                strcpy(shm_ptr->minAvgFileName, file_path);

                printf("MIN AVG file: %s, column: %d\n", shm_ptr->minAvgFileName, shm_ptr->minAvgColumn);

                printf("======================================================\n");


            }

            if (average > shm_ptr->maxAvg) {

                printf("======================================================\n");
                printf("Max average updated:from %.2f to %.2f\n", shm_ptr->maxAvg, average);

                shm_ptr->maxAvg = average;
                shm_ptr->maxAvgColumn = i;
                strcpy(shm_ptr->maxAvgFileName, file_path);

                printf("MAX AVG file: %s, column: %d\n", shm_ptr->maxAvgFileName, shm_ptr->maxAvgColumn);

                printf("======================================================\n");


            }

        }



        int base_time = 30; // Base processing time
        double weight_rows = 0.5; // Weight for rows
        double weight_cols = 0.3; // Weight for columns
        int processingTime = base_time + (int)(weight_rows * rows + weight_cols * cols);

        // Ensure processing time is within reasonable bounds
        if (processingTime < 10) processingTime = 10; // Minimum time
        if (processingTime > 120) processingTime = 120; // Maximum time

        shm_ptr->totalProcessed++;
        semaphore_signal(sem);


        sleep(processingTime);

        send_message(msg_queue_id, 2, file_path); // File mover notifications
        printf("Notification sent to mover: %s\n", file_path);

        // Calculate dynamic processing time based on number of rows
        // ========================


        printf("Processing time for file: %d seconds\n", processingTime);

        // Sleep to ensure there is enough delay after processing the file
//        sleep(processingTime); // Sleep dynamically based on the number of rows
    }
}