#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

void display_memory_info() {
    // Open the kernel's memory info file
    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        perror("MiniOS: Could not open /proc/meminfo");
        return;
    }

    char line[256];
    long total_mem = 0, free_mem = 0, available_mem = 0;

    // Read the file line by line to extract the values we need
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %ld kB", &total_mem);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line, "MemFree: %ld kB", &free_mem);
        } else if (strncmp(line, "MemAvailable:", 13) == 0) {
            sscanf(line, "MemAvailable: %ld kB", &available_mem);
        }
    }
    fclose(fp);

    // Calculate used memory
    long used_mem = total_mem - available_mem;

    // Display nicely formatted output (converted from kB to MB)
    printf("\n╔════════════════════════════════════╗\n");
    printf("║          SYSTEM MEMORY             ║\n");
    printf("╠════════════════════════════════════╣\n");
    printf("║ Total Memory:     %8ld MB      ║\n", total_mem / 1024);
    printf("║ Used Memory:      %8ld MB      ║\n", used_mem / 1024);
    printf("║ Available Memory: %8ld MB      ║\n", available_mem / 1024);
    printf("╚════════════════════════════════════╝\n\n");
}
