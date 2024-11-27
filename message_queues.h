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


void send_message(int msg_id, const char* msg_text) {
    struct message msg;
    msg.type = 1;  // Message type
    snprintf(msg.data, sizeof(msg.data), "%s", msg_text);

    if (msgsnd(msg_id, &msg, sizeof(msg.data), 0) == -1) {
        perror("Message sending failed");
        exit(EXIT_FAILURE);
    }
}

// Cleanup message queue
void cleanup_message_queue() {
    int queue_id = msgget(MESSAGE_QUEUE_KEY, IPC_CREAT | 0777);
    if (queue_id != -1) {
        msgctl(queue_id, IPC_RMID, NULL);
    }
}

void receive_message(int msg_id, char *file_path) {
    struct message msg;

    // Receive the message from the message queue
    if (msgrcv(msg_id, &msg, sizeof(msg.data), 0, 0) == -1) {
        perror("Message receiving failed");
        exit(EXIT_FAILURE);
    }

    // Copy the received message to the file_path buffer
    strncpy(file_path, msg.data, MSG_SIZE);

    printf("Received message: %s\n", file_path);  // Print the received file path
}
#endif
