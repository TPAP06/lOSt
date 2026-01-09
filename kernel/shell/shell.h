// kernel/shell/shell.h

#ifndef SHELL_H
#define SHELL_H

// Shell configuration
#define SHELL_PROMPT "> "
#define SHELL_INPUT_MAX 256
#define SHELL_HISTORY_SIZE 20  // Store last 20 commands

// Shell colors
#define SHELL_COLOR_PROMPT COLOR_LIGHT_CYAN
#define SHELL_COLOR_INPUT COLOR_WHITE
#define SHELL_COLOR_OUTPUT COLOR_LIGHT_GREY
#define SHELL_COLOR_ERROR COLOR_LIGHT_RED
#define SHELL_COLOR_SUCCESS COLOR_LIGHT_GREEN
#define SHELL_COLOR_INFO COLOR_YELLOW

// Initialize the shell
void shell_init(void);

// Run the shell (main loop)
void shell_run(void);

// Print shell prompt
void shell_print_prompt(void);

// Execute a command
void shell_execute(const char *input);

// Add command to history
void shell_add_to_history(const char *cmd);

#endif // SHELL_H