#ifndef CONSTANTS_H
#define CONSTANTS_H

// Shared memory key
#define SHARED_MEMORY_KEY 0x1111

// Message queue key
#define MESSAGE_QUEUE_KEY 0x2222

// Semaphore key (used with sem_open)
#define SEMAPHORE_KEY "/sem_key"



// Thresholds for termination
#define PROCESSED_THRESHOLD 100       // Max files processed
#define UNPROCESSED_THRESHOLD 50      // Max unprocessed files
#define MOVED_THRESHOLD 200           // Max files moved to Backup
#define DELETED_THRESHOLD 300         // Max files deleted

#endif
