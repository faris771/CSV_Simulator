#include "header.h"
#include "header.h"
#include "shared_memories.h"
#include "message_queues.h"
#include "semphores.h"
#include "constants.h"
#include "functions.h"
// Function prototypes
void inspect_home_directory(int age_threshold);


int main(int argc, char** argv){

    printf("File inspector_type1 Running...\n");

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <age_threshold_in_seconds>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    return 0;
}