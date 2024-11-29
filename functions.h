#ifndef __FUNCTIONS__
#define __FUNCTIONS__

#include "header.h"
#include "constants.h"

// Function to read arguments from a file
void read_arguments(const char* filename, int* file_generators, int* csv_calculators, int* file_movers, 
                    int* type1_inspectors, int* type2_inspectors, int* type3_inspectors, int* timer_duration
                    ,int*min_rows,int*max_rows,int*min_cols,int*max_cols ,int*min_time_generate,int*max_time_generate
                    ,int*min_value,int*max_value,int*miss_percentage, int* age_threshold) {
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
        } else if (strcmp(key, "TYPE1_INSPECTORS") == 0) {
            *type1_inspectors = atoi(value);
        } else if (strcmp(key, "TYPE2_INSPECTORS") == 0) {
            *type2_inspectors = atoi(value);
        } else if (strcmp(key, "TYPE3_INSPECTORS") == 0) {
            *type3_inspectors = atoi(value);
        }else if (strcmp(key, "TIMER_DURATION") == 0) {
            *timer_duration = atoi(value);
        }
        else if (strcmp(key, "min_rows") == 0) {
            *min_rows = atoi(value);
        }
        else if (strcmp(key, "max_rows") == 0) {
            *max_rows = atoi(value);
        }
        else if (strcmp(key, "min_cols") == 0) {
            *min_cols = atoi(value);
        }
        else if (strcmp(key, "max_cols") == 0) {
            *max_cols = atoi(value);
        }
        else if (strcmp(key, "min_time_generate") == 0) {
            *min_time_generate = atoi(value);
        }
        else if (strcmp(key, "max_time_generate") == 0) {
            *max_time_generate = atoi(value);
        }
        else if (strcmp(key, "min_value") == 0) {
            *min_value = atoi(value);
        }
        else if (strcmp(key, "max_value") == 0) {
          *max_value = atoi(value);}

        else if (strcmp(key, "miss_percentage") == 0) {
          *miss_percentage = atoi(value);}
        else if (strcmp(key, "AGE_THRESHOLD") == 0){
            *age_threshold = atoi(value);}
    }

    fclose(file);
    // Assign default inspector counts if not explicitly provided
    if (*type1_inspectors == 0) {
        *type1_inspectors = DEFAULT_TYPE1_INSPECTORS;
    }
    if (*type2_inspectors == 0) {
        *type2_inspectors = DEFAULT_TYPE2_INSPECTORS;
    }
    if (*type3_inspectors == 0) {
        *type3_inspectors = DEFAULT_TYPE3_INSPECTORS;
    }
}

#endif