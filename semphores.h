#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include "header.h"
#include "constants.h"

// Setup semaphore
sem_t *setup_semaphore() {
    sem_t *sem = sem_open(SEMAPHORE_KEY, O_CREAT, 0777, 1);
    if (sem == SEM_FAILED) {
        perror("Semaphore Creation Error");
        exit(EXIT_FAILURE);
    }
    return sem;
}

// Cleanup semaphore
void cleanup_semaphore() {
    if (sem_unlink(SEMAPHORE_KEY) < 0) {
        perror("Semaphore Cleanup Error");
    }
}

#endif
