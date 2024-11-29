#include "header.h"
#include "shared_memories.h"
#include "message_queues.h"
#include "semphores.h"
#include "constants.h"
#include "functions.h"

int msg_queue_id;
void move_file(const char *file_path);
int main(int argc, char** argv){

    printf("File mover  Running...\n");
    msg_queue_id = setup_message_queue();
//    receive_message(msg_queue_id);

    char file_path[MSG_SIZE];

    while (1) {
        receive_message(msg_queue_id, 2, file_path); // Only process type-2 messages

        printf("Mover received: %s\n", file_path);

        // Move the file to the Processed directory
        move_file(file_path);
    }
   sleep(1000);
    return 0;
}
void move_file(const char *file_path) {
  sleep(6);
    char processed_dir[MSG_SIZE] = "./home/Processed";

    // Create the Processed directory if it doesn't exist
    struct stat st = {0};
    if (stat(processed_dir, &st) == -1) {
        if (mkdir(processed_dir, 0777) == -1) {
            perror("Error creating Processed directory");
            return;
        }
    }

    // Construct the new file path in the Processed directory
    char *filename = strrchr(file_path, '/');
    if (!filename) return; // Invalid file path
    filename++; // Skip '/'

    char new_path[MSG_SIZE];
    snprintf(new_path, sizeof(new_path), "%s/%s", processed_dir, filename);

    // Move the file
    if (rename(file_path, new_path) == 0) {
        printf("File moved to: %s\n", new_path);
    } else {
        perror("Error moving file");
    }
}