#include "header.h"
#include "shared_memories.h"
#include "semphores.h"
#include "constants.h"
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

sem_t *sem; // Declare the semaphore

void ensure_directory(const char *path);

int main(int argc, char **argv) {
    printf("Inspector Type 1 Running...\n");
    if (argc != 2) {
        fprintf(stderr, "Usage: inspector_type1 <age_threshold_in_seconds>\n");
        exit(EXIT_FAILURE);
    }
    //age_threshold argument, which specifies the maximum file age in seconds before a file is considered unprocessed.
    // Parse age threshold from arguments
    int age_threshold = atoi(argv[1]);

    // Initialize the semaphore
    sem = setup_semaphore();

    // Ensure that the "home" and "UnProcessed" directories exist
    // if directories dont exist then they are created
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
        char file_path[MSG_SIZE];
        char new_path[MSG_SIZE];

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

            // Construct the full file path
            snprintf(file_path, sizeof(file_path), "%s/%s", HOME_DIR, entry->d_name);

            // Get file statistics
            if (stat(file_path, &file_stat) == -1) {
                perror("Error retrieving file stats");
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
                    continue;
                }

                // Move the file to "UnProcessed" directory
                snprintf(new_path, sizeof(new_path), "./home/UnProcessed/%s", entry->d_name);
                if (rename(file_path, new_path) == -1) {
                    perror("Error moving file to UnProcessed");
                } else {
                    printf("File moved to UnProcessed: %s\n", new_path);
                    shm_ptr->totalUnprocessed++; // Update shared memory count
                }

                semaphore_signal(sem);
            }
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
