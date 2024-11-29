#ifndef CONSTANTS_H
#define CONSTANTS_H

// Shared memory key
#define SHARED_MEMORY_KEY 0x1111

// Message queue key
#define MESSAGE_QUEUE_KEY 0x2222
#define HOME_DIR "./home"
// #define DEFAULT_ROWS_MAX 10000
// #define DEFAULT_COLS_MAX 10
// #define DEFAULT_ROWS_MIN 100
// #define DEFAULT_COLS_MIN 1
//////// put small values initially so you can check the averages and see if calculator is done correctly
#define DEFAULT_ROWS_MAX 10
#define DEFAULT_COLS_MAX 4
#define DEFAULT_ROWS_MIN 4
#define DEFAULT_COLS_MIN 1


#define DEFAULT_MIN_VALUE 0.0
#define DEFAULT_MAX_VALUE 100.0
#define DEFAULT_MISS_PERCENTAGE 0.0
#define DEFAULT_GENERATORS 5
#define DEFAULT_MAX_TIME 4
#define DEFAULT_MIN_TIME 1
#define MSG_SIZE 512
// #define  MAX_FILES 100
#define  MAX_FILES 10
#define  MAX_COLUMNS 100

//default values for number of inspectors for each type
#define DEFAULT_TYPE1_INSPECTORS 2
#define DEFAULT_TYPE2_INSPECTORS 2
#define DEFAULT_TYPE3_INSPECTORS 2



// Semaphore key (used with sem_open)
#define SEMAPHORE_KEY "/sem_key"



// Thresholds for termination
#define PROCESSED_THRESHOLD 100       // Max files processed
#define UNPROCESSED_THRESHOLD 50      // Max unprocessed files
#define MOVED_THRESHOLD 200           // Max files moved to Backup
#define DELETED_THRESHOLD 300         // Max files deleted

#endif