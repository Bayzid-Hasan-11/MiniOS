#include <stdio.h>
#include <sys/stat.h> // Required for the stat() system call
#include <time.h>     // Required to format the timestamp
#include "filesystem.h"
#include <dirent.h>
#include <string.h>

// --- WOW FEATURE: Recursive Directory Tree ---
void display_tree(const char *base_path, int depth) {
    DIR *dir = opendir(base_path);
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files and current/parent directory links
        if (entry->d_name[0] == '.') continue;

        // Print indentation based on depth
        for (int i = 0; i < depth; i++) printf("  │ ");
        
        if (entry->d_type == DT_DIR) {
            printf("  ├── \033[1;34m%s\033[0m (Dir)\n", entry->d_name); // Blue for folders
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);
            display_tree(path, depth + 1); // Recursive call
        } else {
            printf("  ├── \033[1;32m%s\033[0m\n", entry->d_name); // Green for files
        }
    }
    closedir(dir);
}

void display_file_info(const char *filename) {
    struct stat file_stat;

    // The stat() system call populates the file_stat struct
    if (stat(filename, &file_stat) == -1) {
        perror("MiniOS: Could not get file info");
        return;
    }

    printf("\n");
    printf("File: %s\n", filename);
    printf("Size: %ld bytes\n", file_stat.st_size);
    printf("Inode: %ld\n", file_stat.st_ino);
    
    // Bitwise operations to extract file permissions (rwx)
    printf("Permissions: ");
    printf((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
    printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");

    // ctime converts the raw UNIX timestamp into a readable string
    printf("Last modified: %s", ctime(&file_stat.st_mtime));
    printf("\n");
}
