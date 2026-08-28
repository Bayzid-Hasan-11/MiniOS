#ifndef SHELL_H
#define SHELL_H

// Returns 1 if the command was a built-in and handled, 0 otherwise
int execute_builtin(char **args);

#endif
