#include "header.h"

#include <dirent.h>

int main(int argc, char** argv){

    printf("File inspector_type2 Running...\n");

    // get current time
    time_t current_time = time(NULL);
    printf("Current time: %s", ctime(&current_time));

    // DIR *d;
    struct dirent *dir;
    d = opendir("home/Processed");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }



    // get the age of the file







    return 0;
}