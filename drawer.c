#include "header.h"
#include "shared_memories.h"
#include "semphores.h"
#include "constants.h"

// Function prototypes
void display(void);
void reshape(int w, int h);
void timer(int z);
void drawDirectories();
void drawFiles();

void drawStatistics();
void drawWarnings();
void drawThresholds();
void readDataFromSharedMemory();

// Shared memory variables
int shm_id;
SharedMemory shm_ptr;
sem_t *sem;

// Variables to hold shared memory data
int totalGenerated = 0;
int totalProcessed = 0;
int totalUnprocessed = 0;
int totalMoved = 0;
int totalDeleted = 0;
int totalBackUp = 0;

double minAvg = 0.0;
double maxAvg = 0.0;
char minAvgFileName[FILENAME_MAX] = "";
char maxAvgFileName[FILENAME_MAX] = "";
int minAvgColumn = -1;
int maxAvgColumn = -1;

// Parameters for visualization
int frame_rate = 1000 / 60; // 60 FPS

// Threshold tracking
int processedThreshold = PROCESSED_THRESHOLD;
int unprocessedThreshold = UNPROCESSED_THRESHOLD;
int movedThreshold = MOVED_THRESHOLD;
int deletedThreshold = DELETED_THRESHOLD;

int main(int argc, char** argv) {
    // Initialize semaphore
    sem = setup_semaphore();

    // Initialize shared memory
    shm_id = shmget(SHARED_MEMORY_KEY, sizeof(struct shared_memory), 0666);
    if (shm_id == -1) {
        perror("Error accessing shared memory");
        exit(EXIT_FAILURE);
    }
    shm_ptr = attach_shared_memory(shm_id);

    // Initialize OpenGL
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("Simulation Drawer");

    // Set up callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);

    // Start the main loop
    glutMainLoop();

    // Cleanup (this will not be reached in normal execution)
    detach_shared_memory(shm_ptr);
    cleanup_semaphore();

    return 0;
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    drawDirectories();
    drawFiles();
    // drawGenerators();
    // drawInspectors();
    drawStatistics();
    drawThresholds();
    drawWarnings();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-40, 40, -30, 30);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int z) {
    glutTimerFunc(frame_rate, timer, 0);
    readDataFromSharedMemory();  // Read shared memory before updating display
    glutPostRedisplay();
}

void drawDirectories() {
    // Draw Backup directory (replacing Home)
    glColor3f(0.96, 0.96, 0.86);  // Beige
    glBegin(GL_QUADS);
    glVertex2f(-35, 20);
    glVertex2f(-25, 20);
    glVertex2f(-25, 10);
    glVertex2f(-35, 10);
    glEnd();

    // Label Backup
    glColor3f(1.0, 1.0, 1.0);  // White text
    glRasterPos2f(-34.5, 21.5);  // Adjusted position for visibility
    const char* backupLabel = "Backup";
    for (const char* c = backupLabel; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Draw Processed directory
    glColor3f(0.96, 0.96, 0.86);  // Beige
    glBegin(GL_QUADS);
    glVertex2f(-35, 5);
    glVertex2f(-25, 5);
    glVertex2f(-25, -5);
    glVertex2f(-35, -5);
    glEnd();

    // Label Processed
    glColor3f(1.0, 1.0, 1.0);  // White text
    glRasterPos2f(-34.5, 6.5);  // Adjusted position for visibility
    const char* processedLabel = "Processed";
    for (const char* c = processedLabel; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Draw UnProcessed directory
    glColor3f(0.96, 0.96, 0.86);  // Beige
    glBegin(GL_QUADS);
    glVertex2f(-35, -10);
    glVertex2f(-25, -10);
    glVertex2f(-25, -20);
    glVertex2f(-35, -20);
    glEnd();

    // Label UnProcessed
    glColor3f(1.0, 1.0, 1.0);  // White text
    glRasterPos2f(-34.5, -8.5);  // Adjusted position for visibility
    const char* unprocessedLabel = "UnProcessed";
    for (const char* c = unprocessedLabel; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}
void drawFiles() {
    // Represent files in the Backup directory
    glColor3f(0.5, 0.5, 0.5); // Gray
    glPointSize(5);
    glBegin(GL_POINTS);

    int filesInBackup = totalBackUp;

    for (int i = 0; i < filesInBackup; i++) {
        glVertex2f(-33 + (i % 5) * 2, 15 - (i / 5) * 2);
    }
    glEnd();

    // Represent files in the Processed directory
    glColor3f(0.0, 1.0, 1.0); // Cyan
    glBegin(GL_POINTS);
    int filesInProcessed = totalProcessed;

    for (int i = 0; i < filesInProcessed; i++) {
        glVertex2f(-33 + (i % 5) * 2, 0 - (i / 5) * 2);
    }
    glEnd();

    // Represent files in the UnProcessed directory
    glColor3f(1.0, 0.0, 1.0); // Magenta
    glBegin(GL_POINTS);
    for (int i = 0; i < totalUnprocessed; i++) {
        glVertex2f(-33 + (i % 5) * 2, -15 - (i / 5) * 2);
    }
    glEnd();
}






void drawStatistics() {
    glColor3f(1.0, 1.0, 1.0);
    char stats[256];

    // Display total files
    snprintf(stats, sizeof(stats), "Generated: %d, Processed: %d, Unprocessed: %d, Moved to Backup: %d, Deleted: %d",
             totalGenerated, totalProcessed, totalUnprocessed, totalMoved, totalDeleted);
    glRasterPos2f(-39, 28);
    for (char* c = stats; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Display Min and Max Averages
    char minAvgStr[256];
    snprintf(minAvgStr, sizeof(minAvgStr), "Min Avg: %.2f (File: %s, Column: %d)",
             minAvg, minAvgFileName, minAvgColumn);
    glRasterPos2f(-39, 25);
    for (char* c = minAvgStr; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    char maxAvgStr[256];
    snprintf(maxAvgStr, sizeof(maxAvgStr), "Max Avg: %.2f (File: %s, Column: %d)",
             maxAvg, maxAvgFileName, maxAvgColumn);
    glRasterPos2f(-39, 23);
    for (char* c = maxAvgStr; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void drawWarnings() {
    // Check thresholds and display warnings
    if (totalProcessed >= processedThreshold) {
        glColor3f(1.0, 0.0, 0.0); // Red
        glRasterPos2f(-10, 25);
        const char* msg = "Processed Threshold Exceeded!";
        for (const char* c = msg; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }

    if (totalUnprocessed >= unprocessedThreshold) {
        glColor3f(1.0, 0.0, 0.0); // Red
        glRasterPos2f(-10, 22);
        const char* msg = "Unprocessed Threshold Exceeded!";
        for (const char* c = msg; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }

    if (totalMoved >= movedThreshold) {
        glColor3f(1.0, 0.0, 0.0); // Red
        glRasterPos2f(-10, 19);
        const char* msg = "Moved to Backup Threshold Exceeded!";
        for (const char* c = msg; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }

    if (totalDeleted >= deletedThreshold) {
        glColor3f(1.0, 0.0, 0.0); // Red
        glRasterPos2f(-10, 16);
        const char* msg = "Deleted Threshold Exceeded!";
        for (const char* c = msg; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }
}
void drawThresholds() {
    glColor3f(1.0, 1.0, 1.0);

    // Display thresholds
    char thresholds[256];
    snprintf(thresholds, sizeof(thresholds), "Processed Threshold: %d", processedThreshold);
    glRasterPos2f(20, 15); // Adjusted to the right
    for (char* c = thresholds; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    snprintf(thresholds, sizeof(thresholds), "UnProcessed Threshold: %d", unprocessedThreshold);
    glRasterPos2f(20, 13); // Adjusted to the right
    for (char* c = thresholds; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    snprintf(thresholds, sizeof(thresholds), "Moved to Backup Threshold: %d", movedThreshold);
    glRasterPos2f(20, 11); // Adjusted to the right
    for (char* c = thresholds; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    snprintf(thresholds, sizeof(thresholds), "Deleted Threshold: %d", deletedThreshold);
    glRasterPos2f(20, 9); // Adjusted to the right
    for (char* c = thresholds; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Display values from arguments.txt
    char arguments[256];
    snprintf(arguments, sizeof(arguments), "CSV Calculators: %d", 4); // Replace 4 with the actual value
    glRasterPos2f(20, 6); // Adjusted to the right
    for (char* c = arguments; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    snprintf(arguments, sizeof(arguments), "File Movers: %d", 2); // Replace 2 with the actual value
    glRasterPos2f(20, 4); // Adjusted to the right
    for (char* c = arguments; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    snprintf(arguments, sizeof(arguments), "Type 1 Inspectors: %d", 2); // Replace 2 with the actual value
    glRasterPos2f(20, 2); // Adjusted to the right
    for (char* c = arguments; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    snprintf(arguments, sizeof(arguments), "Type 2 Inspectors: %d", 2); // Replace 2 with the actual value
    glRasterPos2f(20, 0); // Adjusted to the right
    for (char* c = arguments; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    snprintf(arguments, sizeof(arguments), "Type 3 Inspectors: %d", 2); // Replace 2 with the actual value
    glRasterPos2f(20, -2); // Adjusted to the right
    for (char* c = arguments; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}





void readDataFromSharedMemory() {
    // Synchronize access to shared memory
    semaphore_wait(sem);

    // Update variables from shared memory
    totalGenerated = shm_ptr->totalGenerated;
    totalProcessed = shm_ptr->totalProcessed;
    totalUnprocessed = shm_ptr->totalUnprocessed;
    totalMoved = shm_ptr->totalBackup;   // Assuming totalMoved corresponds to totalBackup
    totalDeleted = shm_ptr->totalDeleted;
    minAvg = shm_ptr->minAvg;
    maxAvg = shm_ptr->maxAvg;
    minAvgColumn = shm_ptr->minAvgColumn;
    maxAvgColumn = shm_ptr->maxAvgColumn;
    totalBackUp = shm_ptr->totalBackup;



    strcpy(minAvgFileName, shm_ptr->minAvgFileName);
    strcpy(maxAvgFileName, shm_ptr->maxAvgFileName);

    // Release the semaphore
    semaphore_signal(sem);
}