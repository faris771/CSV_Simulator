#include "header.h"
#include "shared_memories.h"
#include "message_queues.h"
#include "semphores.h"
#include "constants.h"
#include "functions.h"

// Function prototypes
void setup_resources();
void cleanup_resources();
void fork_processes(int file_generators, int csv_calculators, int file_movers, int inspectors);
void monitor_simulation(int duration, int processed_threshold, int unprocessed_threshold, int moved_threshold, int deleted_threshold);
void create_drawer_process();
void handle_signal(int sig);

// Main program
int main(int argc, char** argv) {
    // Argument variables
    int file_generators, csv_calculators, file_movers, inspectors, timer_duration;

    // Read arguments from file
    read_arguments("arguments.txt", &file_generators, &csv_calculators, &file_movers, &inspectors, &timer_duration);

    printf("Arguments loaded: FILE_GENERATORS=%d, CSV_CALCULATORS=%d, FILE_MOVERS=%d, INSPECTORS=%d, TIMER_DURATION=%d\n",
           file_generators, csv_calculators, file_movers, inspectors, timer_duration);

    // Handle signal for cleanup
    signal(SIGINT, handle_signal);

    // Setup shared resources
    setup_resources();

    // Create the drawer process
    create_drawer_process();

    // Fork processes
    fork_processes(file_generators, csv_calculators, file_movers, inspectors);

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

// Fork processes
void fork_processes(int file_generators, int csv_calculators, int file_movers, int inspectors) {
    printf("Forking processes...\n");

    pid_t pid;

    // Fork file generators
    for (int i = 0; i < file_generators; i++) {
        if ((pid = fork()) == 0) {
            execlp("./file_generator", "file_generator", NULL);
            perror("File Generator Exec Error");
            exit(EXIT_FAILURE);
        }
    }

    // Fork CSV calculators
    for (int i = 0; i < csv_calculators; i++) {
        if ((pid = fork()) == 0) {
            execlp("./csv_calculator", "csv_calculator", NULL);
            perror("CSV Calculator Exec Error");
            exit(EXIT_FAILURE);
        }
    }

    // Fork file movers
    for (int i = 0; i < file_movers; i++) {
        if ((pid = fork()) == 0) {
            execlp("./file_mover", "file_mover", NULL);
            perror("File Mover Exec Error");
            exit(EXIT_FAILURE);
        }
    }

    // Fork inspectors
    for (int i = 0; i < inspectors; i++) {
        if ((pid = fork()) == 0) {
            char inspector_type[32];
            sprintf(inspector_type, "./inspector_type%d", (i % 3) + 1);
            execlp(inspector_type, inspector_type, NULL);
            perror("Inspector Exec Error");
            exit(EXIT_FAILURE);
        }
    }

    printf("Processes forked successfully.\n");
}
