#include "header.h"
#include "shared_memories.h"
#include "message_queues.h"
#include "semphores.h"
#include "constants.h"
#include "functions.h"

// Function prototypes
void setup_resources();
void cleanup_resources();
void fork_processes(int file_generators, int csv_calculators, int file_movers, 
    int type1_inspectors, int type2_inspectors, int type3_inspectors, int age_threshold);
void monitor_simulation(int duration, int processed_threshold, int unprocessed_threshold, int moved_threshold, int deleted_threshold);
void create_drawer_process();
void handle_signal(int sig);

// Main program
int main(int argc, char** argv) {
    printf("masa is here\n");
    // Argument variables
    int file_generators, csv_calculators, file_movers, timer_duration,min_rows,
        max_rows,min_cols,max_cols ,min_time_generate,
        max_time_generate;
    double min_value, max_value, miss_percentage;
    int type1_inspectors = 0, type2_inspectors = 0, type3_inspectors = 0;
    int age_threshold = 0;

    // Read arguments from file
    read_arguments("arguments.txt", &file_generators, &csv_calculators, &file_movers, 
                   &type1_inspectors, &type2_inspectors, &type3_inspectors, &timer_duration,
                   &min_rows, &max_rows, &min_cols, &max_cols, &min_time_generate, &max_time_generate,
                   &min_value, &max_value, &miss_percentage, &age_threshold);

    // Print the loaded arguments
    printf("Arguments loaded:\n");
    printf("FILE_GENERATORS=%d\n", file_generators);
    printf("CSV_CALCULATORS=%d\n", csv_calculators);
    printf("FILE_MOVERS=%d\n", file_movers);
    printf("TYPE1_INSPECTORS=%d\n", type1_inspectors);
    printf("TYPE2_INSPECTORS=%d\n", type2_inspectors);
    printf("TYPE3_INSPECTORS=%d\n", type3_inspectors);
    printf("TIMER_DURATION=%d\n", timer_duration);
    printf("MIN_ROWS=%d\n", min_rows);
    printf("MAX_ROWS=%d\n", max_rows);
    printf("MIN_COLS=%d\n", min_cols);
    printf("MAX_COLS=%d\n", max_cols);
    printf("MIN_TIME_GENERATE=%d\n", min_time_generate);
    printf("MAX_TIME_GENERATE=%d\n", max_time_generate);
    printf("MIN_VALUE=%.2f\n", min_value);
    printf("MAX_VALUE=%.2f\n", max_value);
    printf("MISS_PERCENTAGE=%.2f\n", miss_percentage);
    printf("AGE_THRESHOLD=%d\n", age_threshold);

    // Handle signal for cleanup
    signal(SIGINT, handle_signal);

    // Setup shared resources
    setup_resources();

    // Create the drawer process
    create_drawer_process();

    // Fork processes
    fork_processes(file_generators, csv_calculators, file_movers, type1_inspectors, type2_inspectors, type3_inspectors, age_threshold);

    // Monitor simulation
    monitor_simulation(timer_duration, PROCESSED_THRESHOLD, UNPROCESSED_THRESHOLD, MOVED_THRESHOLD, DELETED_THRESHOLD);

    return 0;
}

// Create the drawer process
void create_drawer_process() {
    printf("Creating the drawer process...\n");

    pid_t drawer;

    if ((drawer = fork()) == -1) {
        perror("Drawer fork error\n");
        exit(-1);
    }

    // To let the drawer leave the main code and execute drawer.c
    if (drawer == 0) {
        execlp("./drawer", "drawer", (char *)NULL);

        // If the program doesn't have enough memory, it will raise an error
        perror("Drawer exec Failure\n");
        exit(-1);
    }

    printf("Drawer process created successfully.\n");
}

// Monitor the simulation for termination conditions
void monitor_simulation(int duration, int processed_threshold, int unprocessed_threshold, int moved_threshold, int deleted_threshold) {
    time_t start_time = time(NULL);
    time_t current_time;

    printf("Simulation started. Monitoring for %d minutes...\n", duration);

    while (1) {
        current_time = time(NULL);

        // Check elapsed time
        if (difftime(current_time, start_time) >= duration * 60) {
            printf("Time limit reached. Ending simulation...\n");
            break;
        }

        // Access shared memory to check thresholds
        int shm_id = shmget(SHARED_MEMORY_KEY, sizeof(struct shared_memory), 0666);
        SharedMemory shm = attach_shared_memory(shm_id);
        if (shm->totalProcessed > processed_threshold) {
            printf("Processed file threshold reached (%d files). Ending simulation...\n", processed_threshold);
            break;
        }
        if (shm->totalUnprocessed > unprocessed_threshold) {
            printf("Unprocessed file threshold reached (%d files). Ending simulation...\n", unprocessed_threshold);
            break;
        }
        if (shm->totalMoved > moved_threshold) {
            printf("Moved file threshold reached (%d files). Ending simulation...\n", moved_threshold);
            break;
        }
        if (shm->totalDeleted > deleted_threshold) {
            printf("Deleted file threshold reached (%d files). Ending simulation...\n", deleted_threshold);
            break;
        }

        detach_shared_memory(shm);

        // Sleep for a short time to reduce CPU usage
        sleep(1);
    }

    // Cleanup resources and exit
    cleanup_resources();
    printf("Simulation ended.\n");
    exit(0);
}

// Setup shared resources
void setup_resources() {
    printf("Setting up resources...\n");

    // Initialize shared memory
    int shm_id = setup_shared_memory();
    SharedMemory shm = attach_shared_memory(shm_id);
    shm->totalGenerated = 0;
    shm->totalProcessed = 0;
    shm->totalUnprocessed = 0; // Initialize unprocessed count
    shm->totalMoved = 0;
    shm->totalDeleted = 0; // Initialize deleted count
    shm->fileSerial = 0;
    shm->totalBackup = 0;

    shm->minAvg = POSITIVE_FLOAT_INFINITY;
    shm->minAvgColumn = -1;
    strcpy(shm->minAvgFileName, "NOT YET DECLARED");

    shm->maxAvg = NEGATIVE_FLOAT_INFINITY;
    shm->maxAvgColumn = -1;
    strcpy(shm->maxAvgFileName, "NOT YET DECLARED");



    for (int i = 0; i < MAX_FILES; i++) {
    shm->numRows[i] = 0;

    for (int j = 0; j < MAX_COLUMNS; j++) {
        shm->columnAverages[i][j] = 0.0;
    }



}
    detach_shared_memory(shm);

    // Initialize other IPC resources
    setup_message_queue();
    setup_semaphore();

    printf("Resources setup complete.\n");
}

// Cleanup shared resources
void cleanup_resources() {
    printf("Cleaning up resources...\n");
    cleanup_shared_memory();
    cleanup_message_queue();
    cleanup_semaphore();
    printf("Resources cleaned up.\n");
}

// Handle signal for cleanup
void handle_signal(int sig) {
    printf("Signal %d received. Cleaning up...\n", sig);
    cleanup_resources();
    exit(0);
}

void fork_processes(int file_generators, int csv_calculators, int file_movers, 
    int type1_inspectors, int type2_inspectors, int type3_inspectors, int age_threshold) {
    pid_t pid;

    // Fork file generators
    for (int i = 0; i < file_generators; i++) {
        pid = fork();
        if (pid == 0) {
            printf("File generator %d started\n", i);
            execlp("./file_generator", "file_generator", NULL);
            perror("File Generator Exec Error");
            exit(EXIT_FAILURE);
        }
        else if (pid < 0) {
            perror("Error forking file generator");
            exit(EXIT_FAILURE);
        }
    }

    // Fork CSV calculators
    for (int i = 0; i < csv_calculators; i++) {
        pid = fork();
        if (pid == 0) {
            printf("CSV calculator %d started\n", i);
            execlp("./csv_calculator", "csv_calculator", NULL);
            perror("CSV Calculator Exec Error");
            exit(EXIT_FAILURE);
            sleep(13);
        }
        else if (pid < 0) {
            perror("Error forking CSV calculator");
            exit(EXIT_FAILURE);
        }
        sleep(2);
    }

    // Fork file movers
    for (int i = 0; i < file_movers; i++) {
        pid = fork();
        if (pid == 0) {
            printf("File mover %d started\n", i);
            execlp("./file_mover", "file_mover", NULL);
            perror("File Mover Exec Error");
            exit(EXIT_FAILURE);
            sleep(10);
        }
        else if (pid < 0) {
            perror("Error forking file mover");
            exit(EXIT_FAILURE);
        }
        sleep(2);
    }

    // // Fork inspectors
    // for (int i = 0; i < inspectors; i++) {
    //     pid = fork();
    //     if (pid == 0) {
    //         char inspector_type[32];
    //         sprintf(inspector_type, "./inspector_type%d", (i % 3) + 1);
    //         printf("Inspector %d of type %d started\n", i, (i % 3) + 1);
    //         execlp(inspector_type, inspector_type, NULL);
    //         perror("Inspector Exec Error");
    //         exit(EXIT_FAILURE);
    //     }
    //     else if (pid < 0) {
    //         perror("Error forking inspector");
    //         exit(EXIT_FAILURE);
    //     }
    // }

////////////////////////////////////////////////////////////////

    // Fork Type 1 inspectors
    for (int i = 0; i < type1_inspectors; i++) {
        pid = fork();
        if (pid == 0) {
            printf("Type 1 inspector %d started\n", i + 1);
            char age_arg[16];
            sprintf(age_arg, "%d", age_threshold);
            execlp("./inspector_type1", "inspector_type1", age_arg, NULL); // Pass age_threshold
            perror("Type 1 Inspector Exec Error");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("Error forking Type 1 inspector");
            exit(EXIT_FAILURE);
        }
    }

    // Fork Type 2 inspectors
    for (int i = 0; i < type2_inspectors; i++) {
        pid = fork();
        if (pid == 0) {
            printf("Type 2 inspector %d started\n", i + 1);
            char age_arg[16];
            sprintf(age_arg, "%d", age_threshold);
            execlp("./inspector_type2", "inspector_type2", age_arg, NULL); // Pass age_threshold
            perror("Type 2 Inspector Exec Error");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("Error forking Type 2 inspector");
            exit(EXIT_FAILURE);
        }
    }

    // Fork Type 3 inspectors
    for (int i = 0; i < type3_inspectors; i++) {
        pid = fork();
        if (pid == 0) {
            printf("Type 3 inspector %d started\n", i + 1);
            char age_arg[16];
            sprintf(age_arg, "%d", age_threshold);
            execlp("./inspector_type3", "inspector_type3", age_arg, NULL); // Pass age_threshold
            perror("Type 3 Inspector Exec Error");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("Error forking Type 3 inspector");
            exit(EXIT_FAILURE);
        }
    }

//    pid_t reporter = fork();
//    if (reporter == 0) {
//
//    }
//

    printf("Processes forked successfully.\n");
}

