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

    printf("=================== Inspector Type 3 Running...=================\n");

    printf("FARIS IN THE HOUSE...\n");

    if (argc != 2) {
        fprintf(stderr, "Usage: inspector_type3 <age_threshold_in_seconds>\n");
        exit(EXIT_FAILURE);
    }
    //age_threshold argument, which specifies the maximum file age in seconds before a file is considered unprocessed.
    // Parse age threshold from arguments
    int age_threshold = atoi(argv[1]);

    printf("[INSPECTOR 3] AGE THRESHOLD = %d\n", age_threshold);



    // Initialize the semaphore
    sem = setup_semaphore();

    // Ensure that the "home" and "UnProcessed" directories exist
    // if directories don't exist then they are created

    ensure_directory(HOME_DIR);
    ensure_directory(PROCESSED_DIR);
    ensure_directory(BACKUP_DIR);



    while (1) {


        DIR *backup_dir = opendir(BACKUP_DIR);
        if (!backup_dir) {
            perror("Error opening backup directory");
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
//        printf("[INSPECTOR 3] TOTAL BACKUP: %d\n", shm_ptr->totalBackup);

        time_t current_time = time(NULL);

        // Iterate through files in the "backup" directory

        //  iterate over the BACKUP_DIR


        while ((entry = readdir(backup_dir)) != NULL) {
            // Skip "." and ".." directories
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

            // Skip non-CSV files
            if (strstr(entry->d_name, ".csv") == NULL) continue;

            // Construct the full file path
            snprintf(file_path, sizeof(file_path), "%s/%s", BACKUP_DIR, entry->d_name);


            printf("[INSPECTOR 3] Inspecting file: %s\n", file_path);


            // Get file statistics
            if (stat(file_path, &file_stat) == -1) {
                perror("Error retrieving file stats");
                continue;
            }

            // Check if the file age exceeds the threshold
            double file_age = difftime(current_time, file_stat.st_mtime);
            printf("[INSPECTOR 3] Inspecting file: %s, Age: %.2f seconds\n", file_path, file_age);

            if (file_age > age_threshold) {
                // Extract the file serial number from its name
                int file_serial = atoi(entry->d_name);

                semaphore_wait(sem); // Use semaphore to ensure mutual exclusion

//                // Check if the file has been handled by the calculator
//                if (shm_ptr->numRows[file_serial] > 0) {
//                    printf("File already processed: %s\n", file_path);
//                    semaphore_signal(sem);
//                    continue;
//                }
                // delete the file

                if (remove(file_path) == -1) {
                    perror("Error deleting file");
                } else {
                    printf("File deleted: %s\n", file_path);
                    shm_ptr->totalDeleted++; // Update shared memory count
                    shm_ptr->totalBackup--; // Update shared memory count
                }

                semaphore_signal(sem);


            }
        }

        closedir(backup_dir);

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