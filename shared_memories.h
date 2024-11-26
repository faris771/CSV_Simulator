#ifndef SHARED_MEMORIES_H
#define SHARED_MEMORIES_H

#include "header.h"
#include "constants.h"

// Define the shared memory structure
struct shared_memory {
    int totalGenerated;     // Total CSV files generated
    int totalProcessed;     // Total CSV files processed
    int totalUnprocessed;   // Total CSV files moved to Unprocessed
    int totalMoved;         // Total CSV files moved to Processed
    int totalDeleted;       // Total CSV files deleted
};

typedef struct shared_memory *SharedMemory;

// Initialize shared memory
int setup_shared_memory() {
    int shm_id = shmget(SHARED_MEMORY_KEY, sizeof(struct shared_memory), IPC_CREAT | 0777);
    if (shm_id == -1) {
        perror("Shared Memory Creation Error");
        exit(EXIT_FAILURE);
    }
    return shm_id;
}

// Access shared memory
SharedMemory attach_shared_memory(int shm_id) {
    SharedMemory shm_ptr = (SharedMemory)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("Shared Memory Attach Error");
        exit(EXIT_FAILURE);
    }
    return shm_ptr;
}

// Detach shared memory
void detach_shared_memory(SharedMemory shm_ptr) {
    if (shmdt(shm_ptr) == -1) {
        perror("Shared Memory Detach Error");
        exit(EXIT_FAILURE);
    }
}

// Remove shared memory
void cleanup_shared_memory() {
    int shm_id = shmget(SHARED_MEMORY_KEY, sizeof(struct shared_memory), IPC_CREAT | 0777);
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("Shared Memory Cleanup Error");
    }
}

#endif
