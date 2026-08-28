#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include "shell.h"
#include <sys/utsname.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

// Automatically clean up background processes so they don't become Zombies
void handle_sigchld(int sig) {
    (void)sig; 
    int saved_errno = errno; 
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];

    // Setup Zombie handler
    struct sigaction sa;
    sa.sa_handler = &handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
        perror("MiniOS: sigaction failed");
        exit(1);
    }

    // --- FEATURE 1: Ignore Ctrl+C in the main shell ---
    signal(SIGINT, SIG_IGN);

    // --- WOW FEATURE: MiniFetch Boot Screen ---
    struct utsname sys_info;
    uname(&sys_info); // Ask the kernel for system details
    
    printf("\033[1;36m"); // Set text to Cyan
    printf("    __  ____       _ ____  _____\n");
    printf("   /  |/  (_)___  (_) __ \\/ ___/\n");
    printf("  / /|_/ / / __ \\/ / / / /\\__ \\ \n");
    printf(" / /  / / / / / / / /_/ /___/ / \n");
    printf("/_/  /_/_/_/ /_/_/\\____//____/  \n");
    printf("\033[0m"); // Reset color
    printf("======================================\n");
    printf(" OS: %s %s\n", sys_info.sysname, sys_info.release);
    printf(" Architecture: %s\n", sys_info.machine);
    printf("======================================\n");

    while (1) {
        // --- FEATURE 2: Dynamic Directory Prompt ---
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            // Prints "MiniOS:/current/path> " with professional colors
            printf("\033[1;34mMiniOS:\033[0m\033[1;32m%s\033[0m> ", cwd);
        } else {
            printf("MiniOS> ");
        }
        
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) == 0) continue;
        
        if (strcmp(input, "exit") == 0) {
            printf("Exiting MiniOS...\n");
            break;
        }

        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        int background = 0;
        if (i > 0 && strcmp(args[i - 1], "&") == 0) {
            background = 1;
            args[i - 1] = NULL; 
            i--;
        }

        // --- CHECK FOR PIPES '|' ---
        int pipe_idx = -1;
        for (int j = 0; j < i; j++) {
            if (strcmp(args[j], "|") == 0) {
                pipe_idx = j;
                break;
            }
        }

        if (pipe_idx != -1) {
            args[pipe_idx] = NULL;
            char **left_cmd = args;
            char **right_cmd = &args[pipe_idx + 1];

            int fd[2];
            if (pipe(fd) == -1) continue;

            if (fork() == 0) {
                signal(SIGINT, SIG_DFL); // Restore Ctrl+C for child
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
                execvp(left_cmd[0], left_cmd);
                exit(1);
            }

            if (fork() == 0) {
                signal(SIGINT, SIG_DFL); // Restore Ctrl+C for child
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                close(fd[1]);
                execvp(right_cmd[0], right_cmd);
                exit(1);
            }

            close(fd[0]);
            close(fd[1]);
            wait(NULL);
            wait(NULL);
            continue;
        }

        // --- CHECK FOR I/O REDIRECTION ---
        char *output_file = NULL;
        char *input_file = NULL;

        for (int j = 0; j < i; j++) {
            if (strcmp(args[j], ">") == 0) {
                output_file = args[j + 1];
                args[j] = NULL; 
            } else if (strcmp(args[j], "<") == 0) {
                input_file = args[j + 1];
                args[j] = NULL;
            }
        }

        // --- NORMAL COMMAND EXECUTION ---
        if (execute_builtin(args)) continue; 

        pid_t pid = fork();
        if (pid < 0) {
            perror("MiniOS: Fork failed");
        } 
        else if (pid == 0) {
            // Restore Ctrl+C for standard child processes
            signal(SIGINT, SIG_DFL); 

            if (output_file != NULL) {
                int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd_out < 0) {
                    perror("MiniOS: failed to open output file");
                    exit(1);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }

            if (input_file != NULL) {
                int fd_in = open(input_file, O_RDONLY);
                if (fd_in < 0) {
                    perror("MiniOS: failed to open input file");
                    exit(1);
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }

            if (execvp(args[0], args) == -1) {
                perror("MiniOS: Command not found");
            }
            exit(EXIT_FAILURE);
        } 
        else {
            if (background) {
                printf("[Running in background] PID: %d\n", pid);
            } else {
                int status;
                waitpid(pid, &status, 0);
            }
        }
    }
    return 0;
}
