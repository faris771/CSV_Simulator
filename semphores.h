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


void semaphore_wait(sem_t *sem) {
    if (sem_wait(sem) == -1) {
        perror("Semaphore wait failed");
        exit(EXIT_FAILURE);
    }
}

void semaphore_signal(sem_t *sem) {
    if (sem_post(sem) == -1) {
        perror("Semaphore signal failed");
        exit(EXIT_FAILURE);
    }
}


// Cleanup semaphore
void cleanup_semaphore() {
    if (sem_unlink(SEMAPHORE_KEY) < 0) {
        perror("Semaphore Cleanup Error");
    }
}

#endif
