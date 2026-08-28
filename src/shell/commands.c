#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/statvfs.h> // REQUIRED FOR DISK SPACE (statvfs)
#include "shell.h"
#include "process.h"
#include "memory.h"
#include "filesystem.h"
#include "sync.h"

int execute_builtin(char **args) {
    if (args[0] == NULL) {
        return 1;
    }

    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "MiniOS: cd requires an argument\n");
        } else {
            if (chdir(args[1]) != 0) {
                perror("MiniOS: cd failed");
            }
        }
        return 1;
    }
    
    if (strcmp(args[0], "processes") == 0) {
        list_processes();
        return 1;
    }

    if (strcmp(args[0], "kill") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "MiniOS: kill requires a PID (e.g., kill 1234)\n");
        } else {
            pid_t target_pid = atoi(args[1]);
            if (kill(target_pid, SIGTERM) == 0) {
                printf("MiniOS: Sent SIGTERM to process %d\n", target_pid);
            } else {
                perror("MiniOS: kill failed");
            }
        }
        return 1;
    }

    // --- WOW FEATURE: Freeze a process ---
    if (strcmp(args[0], "pause") == 0) {
        if (args[1] != NULL) {
            pid_t pid = atoi(args[1]);
            kill(pid, SIGSTOP); // OS signal to suspend execution
            printf("MiniOS: \033[1;33mPaused\033[0m process %d\n", pid);
        }
        return 1;
    }

    // --- WOW FEATURE: Unfreeze a process ---
    if (strcmp(args[0], "resume") == 0) {
        if (args[1] != NULL) {
            pid_t pid = atoi(args[1]);
            kill(pid, SIGCONT); // OS signal to resume execution
            printf("MiniOS: \033[1;32mResumed\033[0m process %d\n", pid);
        }
        return 1;
    }

    if (strcmp(args[0], "memory") == 0) {
        display_memory_info();
        return 1;
    }

    if (strcmp(args[0], "info") == 0) {
        if (args[1] == NULL) {
            fprintf(stderr, "MiniOS: info requires a filename (e.g., info file.txt)\n");
        } else {
            display_file_info(args[1]);
        }
        return 1;
    }

    if (strcmp(args[0], "sync-demo") == 0) {
        run_sync_demo();
        return 1;
    }

    // --- FEATURE 3: DISK SPACE MONITOR ---
    if (strcmp(args[0], "disk") == 0) {
        struct statvfs vfs;
        if (statvfs(".", &vfs) == 0) {
            long block_size = vfs.f_frsize;
            long total_space = (vfs.f_blocks * block_size) / (1024 * 1024 * 1024); // in GB
            long free_space = (vfs.f_bfree * block_size) / (1024 * 1024 * 1024);   // in GB
            long used_space = total_space - free_space;

            printf("\n╔════════════════════════════════════╗\n");
            printf("║          DISK USAGE (GB)           ║\n");
            printf("╠════════════════════════════════════╣\n");
            printf("║ Total Disk Space: %8ld GB      ║\n", total_space);
            printf("║ Used Space:       %8ld GB      ║\n", used_space);
            printf("║ Free Space:       %8ld GB      ║\n", free_space);
            printf("╚════════════════════════════════════╝\n\n");
        } else {
            perror("MiniOS: statvfs failed");
        }
        return 1;
    }

    if (strcmp(args[0], "tree") == 0) {
        printf("\033[1;34m.\033[0m\n");
        display_tree(".", 0);
        return 1;
    }

    return 0; 
}
