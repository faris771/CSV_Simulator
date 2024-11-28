#include "header.h"
#include "shared_memories.h"
#include "constants.h"

int main() {
    // Access shared memory
    int shm_id = shmget(SHARED_MEMORY_KEY, sizeof(struct shared_memory), 0666);
    if (shm_id == -1) {
        perror("Error accessing shared memory");
        exit(EXIT_FAILURE);
    }

    SharedMemory shm_ptr = attach_shared_memory(shm_id);

    // Print the total numbers from the shared memory
    printf("Reading from shared memory:\n");
    printf("Total Generated Files: %d\n", shm_ptr->totalGenerated);
    printf("Total Processed Files: %d\n", shm_ptr->totalProcessed);
    printf("Total Unprocessed Files: %d\n", shm_ptr->totalUnprocessed);
    printf("Total Moved Files: %d\n", shm_ptr->totalMoved);
    printf("Total Deleted Files: %d\n", shm_ptr->totalDeleted);

    // Detach shared memory
    detach_shared_memory(shm_ptr);

    return 0;
}
