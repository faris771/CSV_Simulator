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
        // Wait for the file path from the message queue and store it in file_path
        receive_message(msg_queue_id, file_path);

        // Extract file serial number from the filename
        char *filename = strrchr(file_path, '/');
        if (filename == NULL) continue;
        filename++; // Skip the '/' character

        int fileSerial;
        if (sscanf(filename, "%d.csv", &fileSerial) != 1) continue;

        // Open the CSV file
        FILE *file = fopen(file_path, "r");
        if (!file) {
            perror("Error opening CSV file");
            continue;
        }

        // Processing logic (e.g., calculating averages)
        int rows = 0, cols = 0;
        double columnSums[MAX_COLUMNS] = {0};
        int columnCounts[MAX_COLUMNS] = {0};
        char line[1024];

        if (fgets(line, sizeof(line), file) != NULL) {
            char *token = strtok(line, ",");
            while (token != NULL) {
                cols++;
                token = strtok(NULL, ",");
            }
        }

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
        for (int i = 0; i < cols; i++) {
            shm_ptr->columnAverages[fileSerial][i] = (columnCounts[i] > 0) ? columnSums[i] / columnCounts[i] : 0;
        }
        shm_ptr->totalProcessed++;
        semaphore_signal(sem);

        // Notify the mover that the file has been processed
        send_message(msg_queue_id, file_path);
        printf("Notification sent to mover: %s\n", file_path);
    }
}

