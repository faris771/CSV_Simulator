#ifndef MESSAGE_QUEUES_H
#define MESSAGE_QUEUES_H

#include "header.h"
#include "constants.h"

// Message structure
struct message {
    long type;
    char data[256];
};

typedef struct message Message;

// Setup message queue
int setup_message_queue() {
    int queue_id = msgget(MESSAGE_QUEUE_KEY, IPC_CREAT | 0777);
    if (queue_id == -1) {
        perror("Message Queue Creation Error");
        exit(EXIT_FAILURE);
    }
    return queue_id;
}

// Cleanup message queue
void cleanup_message_queue() {
    int queue_id = msgget(MESSAGE_QUEUE_KEY, IPC_CREAT | 0777);
    if (queue_id != -1) {
        msgctl(queue_id, IPC_RMID, NULL);
    }
}

#endif
