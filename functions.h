#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#include "header.h"
#include "constants.h"

// Function to read arguments from a file
void read_arguments(const char* filename, int* file_generators, int* csv_calculators, int* file_movers, int* inspectors, int* timer_duration) {
    char line[200];
    char key[50];
    char value[50];

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening arguments file");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        // Parse key-value pairs
        sscanf(line, "%[^=]=%s", key, value);

        // Match keys to variables
        if (strcmp(key, "FILE_GENERATORS") == 0) {
            *file_generators = atoi(value);
        } else if (strcmp(key, "CSV_CALCULATORS") == 0) {
            *csv_calculators = atoi(value);
        } else if (strcmp(key, "FILE_MOVERS") == 0) {
            *file_movers = atoi(value);
        } else if (strcmp(key, "INSPECTORS") == 0) {
            *inspectors = atoi(value);
        } else if (strcmp(key, "TIMER_DURATION") == 0) {
            *timer_duration = atoi(value);
        }
    }

    fclose(file);
}

#endif
