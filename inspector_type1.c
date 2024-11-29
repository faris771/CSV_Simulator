#include "header.h"
#include "shared_memories.h"
#include "semphores.h"
#include "constants.h"
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h> // For malloc and free
#include <stdio.h>  // For snprintf and perror

sem_t *sem; // Declare the semaphore

void ensure_directory(const char *path);

int main(int argc, char **argv) {
    printf("Inspector Type 1 Running...\n");

    if (argc != 2) {
        fprintf(stderr, "Usage: inspector_type1 <age_threshold_in_seconds>\n");
        exit(EXIT_FAILURE);
    }

    // Parse age threshold from arguments
    int age_threshold = atoi(argv[1]);

    // Initialize the semaphore
    sem = setup_semaphore();

    // Ensure that the "home" and "UnProcessed" directories exist
    ensure_directory(HOME_DIR);
    ensure_directory("./home/UnProcessed");

    while (1) {
        DIR *home_dir = opendir(HOME_DIR);
        if (!home_dir) {
            perror("Error opening home directory");
            exit(EXIT_FAILURE);
        }

        struct dirent *entry;
        struct stat file_stat;

        // Access shared memory
        int shm_id = shmget(SHARED_MEMORY_KEY, sizeof(struct shared_memory), 0666);
        if (shm_id == -1) {
            perror("Shared memory access error");
            exit(EXIT_FAILURE);
        }
        SharedMemory shm_ptr = attach_shared_memory(shm_id);

        time_t current_time = time(NULL);

        // Iterate through files in the "home" directory
        while ((entry = readdir(home_dir)) != NULL) {
            // Skip "." and ".." directories
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

            // Skip non-CSV files
            if (strstr(entry->d_name, ".csv") == NULL) continue;

            // Allocate memory for file paths
            char *file_path = malloc(strlen(HOME_DIR) + strlen(entry->d_name) + 2); // +2 for '/' and '\0'
            char *new_path = malloc(strlen("./home/UnProcessed/") + strlen(entry->d_name) + 1); // +1 for '\0'

            if (!file_path || !new_path) {
                perror("Memory allocation failed");
                free(file_path);
                free(new_path);
                continue;
            }

            // Construct the full file path
            if (snprintf(file_path, strlen(HOME_DIR) + strlen(entry->d_name) + 2, "%s/%s", HOME_DIR, entry->d_name) >= (strlen(HOME_DIR) + strlen(entry->d_name) + 2)) {
                fprintf(stderr, "Error: file_path buffer too small for %s/%s\n", HOME_DIR, entry->d_name);
                free(file_path);
                free(new_path);
                continue;
            }

            // Get file statistics
            if (stat(file_path, &file_stat) == -1) {
                perror("Error retrieving file stats");
                free(file_path);
                free(new_path);
                continue;
            }

            // Check if the file age exceeds the threshold
            double file_age = difftime(current_time, file_stat.st_mtime);
            printf("Inspecting file: %s, Age: %.2f seconds\n", file_path, file_age);

            if (file_age > age_threshold) {
                // Extract the file serial number from its name
                int file_serial = atoi(entry->d_name);

                semaphore_wait(sem); // Use semaphore to ensure mutual exclusion

                // Check if the file has been handled by the calculator
                if (shm_ptr->numRows[file_serial] > 0) {
                    printf("File already processed: %s\n", file_path);
                    semaphore_signal(sem);
                    free(file_path);
                    free(new_path);
                    continue;
                }

                // Move the file to "UnProcessed" directory
                if (snprintf(new_path, strlen("./home/UnProcessed/") + strlen(entry->d_name) + 1, "./home/UnProcessed/%s", entry->d_name) >= (strlen("./home/UnProcessed/") + strlen(entry->d_name) + 1)) {
                    fprintf(stderr, "Error: new_path buffer too small for ./home/UnProcessed/%s\n", entry->d_name);
                    free(file_path);
                    free(new_path);
                    semaphore_signal(sem);
                    continue;
                }

                if (rename(file_path, new_path) == -1) {
                    perror("Error moving file to UnProcessed");
                } else {
                    printf("File moved to UnProcessed: %s\n", new_path);
                    shm_ptr->totalUnprocessed++; // Update shared memory count
                }

                semaphore_signal(sem);
            }

            // Free memory for file paths
            free(file_path);
            free(new_path);
        }

        closedir(home_dir);

        // Small delay before rechecking
        sleep(1);
    }

    return 0;
}

// Ensure a directory exists, create it if it doesn't
void ensure_directory(const char *path) {
    if (mkdir(path, 0777) == -1 && errno != EEXIST) {
        perror("Error creating directory");
        exit(EXIT_FAILURE);
    }
}
