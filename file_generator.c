#include "header.h"
#include "shared_memories.h"
#include "message_queues.h"
#include "semphores.h"
#include "constants.h"
#include "functions.h"
sem_t *sem;
static int file_serial = 0;
int shm_id;
int msg_queue_id;
int max_rows = DEFAULT_ROWS_MAX, max_cols = DEFAULT_COLS_MAX, min_rows = DEFAULT_ROWS_MIN, min_cols = DEFAULT_COLS_MIN ; // Default values
double min_value = DEFAULT_MIN_VALUE, max_value = DEFAULT_MAX_VALUE  ;

int type1_inspectors = DEFAULT_TYPE1_INSPECTORS, type2_inspectors = DEFAULT_TYPE2_INSPECTORS,type3_inspectors = DEFAULT_TYPE3_INSPECTORS; 
int file_generators = DEFAULT_GENERATORS, csv_calculators = DEFAULT_CSV_CALCULATORS, file_movers = DEFAULT_FILE_MOVERS, timer_duration = DEFAULT_TIMER_DURATION, age_threshold= DEFAULT_AGE_THRESHOLD;



int max_time_generate = DEFAULT_MAX_TIME , min_time_generate = DEFAULT_MIN_TIME;
double miss_percentage = DEFAULT_MISS_PERCENTAGE;
SharedMemory shm_ptr;

void generate_csv_file();
void update_shared_mem();

int main(int argc, char** argv){

    // Argument variables
    // int file_generators, csv_calculators, file_movers, timer_duration,min_rows,max_rows,
    //     min_cols,max_cols ,min_time_generate,max_time_generate;
    // double min_value,max_value,miss_percentage;               
    // int type1_inspectors = 0, type2_inspectors = 0, type3_inspectors = 0;
    //int age_threshold = 0;
    // Read arguments from file
    read_arguments("arguments.txt", &file_generators, &csv_calculators, &file_movers, 
                   &type1_inspectors, &type2_inspectors, &type3_inspectors, &timer_duration,
                   &min_rows, &max_rows, &min_cols, &max_cols, &min_time_generate, &max_time_generate,
                   &min_value, &max_value, &miss_percentage, &age_threshold);
    
    ////////////////////////////////////////////////////////////////////////////

    printf("MASA CHECK -----> FILE_GENERATORS=%d\n", file_generators);
    printf("MASA CHECK -----> CSV_CALCULATORS=%d\n", csv_calculators);
    printf("MASA CHECK -----> FILE_MOVERS=%d\n", file_movers);
    printf("MASA CHECK -----> TYPE1_INSPECTORS=%d\n", type1_inspectors);
    printf("MASA CHECK -----> TYPE2_INSPECTORS=%d\n", type2_inspectors);
    printf("MASA CHECK -----> TYPE3_INSPECTORS=%d\n", type3_inspectors);
    printf("MASA CHECK -----> TIMER_DURATION=%d\n", timer_duration);
    printf("MASA CHECK -----> MIN_ROWS=%d\n", min_rows);
    printf("MASA CHECK -----> MAX_ROWS=%d\n", max_rows);
    printf("MASA CHECK -----> MIN_COLS=%d\n", min_cols);
    printf("MASA CHECK -----> MAX_COLS=%d\n", max_cols);
    printf("MASA CHECK -----> MIN_TIME_GENERATE=%d\n", min_time_generate);
    printf("MASA CHECK -----> MAX_TIME_GENERATE=%d\n", max_time_generate);
    printf("MASA CHECK -----> MIN_VALUE=%.2f\n", min_value);
    printf("MASA CHECK -----> MAX_VALUE=%.2f\n", max_value);
    printf("MASA CHECK -----> MISS_PERCENTAGE=%.2f\n", miss_percentage);

    ////////////////////////////////////////////////////////////////////////////

    srand(time(NULL) + getpid()); // Seed for randomness
    sem = setup_semaphore();
    shm_id = setup_shared_memory();  // Create shared memory segment
    shm_ptr = attach_shared_memory(shm_id);  // Attach to shared memory
    msg_queue_id = setup_message_queue();

    generate_csv_file();


  sleep(1000);


    return 0;
}

void generate_csv_file() {
    // Calculate random generation time
    int gen_time = min_time_generate + rand() % (max_time_generate - min_time_generate + 1);
    printf("THE min_time_generate IS %d AND THE max_time_generate IS %d and thus the gen_time IS %d\n\n", min_time_generate, max_time_generate, gen_time);
    sleep(gen_time);

    // Create directory if it doesn't exist
    mkdir(HOME_DIR, 0777);

    // File path and serial number
    char file_path[MSG_SIZE];

    // Lock semaphore before accessing shared memory
    semaphore_wait(sem);

    // Fetch and increment the serial number from shared memory
    int serial_number = shm_ptr->fileSerial++;
    snprintf(file_path, sizeof(file_path), "%s/%d.csv", HOME_DIR, serial_number);

    // Unlock semaphore
    semaphore_signal(sem);

    // Open file for writing
    FILE *file = fopen(file_path, "w");
    if (!file) {
        perror("file_generator______Error creating CSV file");
        exit(EXIT_FAILURE);
    }

    // Generate random rows and columns
    int rows = min_rows + rand() % (max_rows - min_rows + 1);
    int cols = min_cols + rand() % (max_cols - min_cols + 1);

    // Write column headers
    for (int j = 0; j < cols; j++) {
        fprintf(file, "Column%d", j + 1);
        if (j < cols - 1) {
            fprintf(file, ",");
        }
    }
    fprintf(file, "\n");

    // Write rows of data
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // Condition for missing values: on diagonal or based on a custom rule
            if (i == j || (i + j) % 5 == 0) {
                // Missing value condition: diagonal or (i + j) is divisible by 5
                fprintf(file, " ,");
            } else {
                // Non-missing value
                fprintf(file, "%.2f", (float)(min_value + rand() %(int)(max_value - min_value + 1)));
            }

            if (j < cols - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("CSV file %s generated successfully with %d rows and %d columns.\n", file_path, rows, cols);

    // Lock semaphore to update shared memory stats
    semaphore_wait(sem);

    shm_ptr->totalGenerated++; // Update total files generated

    semaphore_signal(sem); // Unlock semaphore

 send_message(msg_queue_id, 1, file_path); // File calculator messages
}



// Function to update shared memory with file generation statistics
void update_shared_mem() {
    semaphore_wait( sem);
    // Update shared memory
    shm_ptr->totalGenerated++;

    semaphore_signal (sem);
    detach_shared_memory(shm_ptr);

    printf("Shared memory updated: Total generated files: %d\n", shm_ptr->totalGenerated);
}