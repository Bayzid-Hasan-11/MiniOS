#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include "process.h"

// Helper function to check if a directory name is purely numbers (a PID)
int is_numeric(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

void list_processes() {
    DIR *dir;
    struct dirent *entry;

    // Open the kernel's virtual filesystem
    dir = opendir("/proc");
    if (dir == NULL) {
        perror("MiniOS: Could not open /proc");
        return;
    }

    printf("\n%-10s %-25s %-10s\n", "PID", "COMMAND", "STATE");
    printf("--------------------------------------------------\n");

    // Loop through everything in /proc
    while ((entry = readdir(dir)) != NULL) {
        // If it's a directory and its name is a number, it's a running process
        if (entry->d_type == DT_DIR && is_numeric(entry->d_name)) {
            char path[512];
            snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);

            FILE *file = fopen(path, "r");
            if (file) {
                int pid;
                char comm[256];
                char state;
                
                // The stat file format is: PID (CommandName) State ...
                if (fscanf(file, "%d (%[^)]) %c", &pid, comm, &state) == 3) {
                    
                    // Convert the state char to a readable string
                    char state_str[20];
                    switch(state) {
                        case 'R': strcpy(state_str, "Running"); break;
                        case 'S': strcpy(state_str, "Sleeping"); break;
                        case 'Z': strcpy(state_str, "Zombie"); break;
                        case 'T': strcpy(state_str, "Stopped"); break;
                        case 'I': strcpy(state_str, "Idle"); break;
                        default: snprintf(state_str, sizeof(state_str), "%c", state);
                    }
                    
                    printf("%-10d %-25s %-10s\n", pid, comm, state_str);
                }
                fclose(file);
            }
        }
    }
    closedir(dir);
    printf("\n");
}
