// kernel/shell/commands.h - Built-in commands

#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdbool.h>

// Command handler function type
typedef void (*command_handler_t)(int argc, char **argv);

// Command structure
typedef struct {
    const char *name;
    const char *description;
    command_handler_t handler;
} command_t;

// Initialize commands system
void commands_init(void);

// Execute a command by name
bool commands_execute(const char *name, int argc, char **argv);

// Get all commands (for help)
const command_t *commands_get_all(int *count);

// Parse command line into argc/argv
int commands_parse(char *input, char **argv, int max_args);

// Built-in command handlers
void cmd_help(int argc, char **argv);
void cmd_clear(int argc, char **argv);
void cmd_echo(int argc, char **argv);
void cmd_info(int argc, char **argv);
void cmd_uptime(int argc, char **argv);
void cmd_reboot(int argc, char **argv);
void cmd_shutdown(int argc, char **argv);
void cmd_calc(int argc, char **argv);
void cmd_color(int argc, char **argv);
void cmd_meminfo(int argc, char **argv);
void cmd_uptime(int argc, char **argv);
void cmd_date(int argc, char **argv);
void cmd_sleep(int argc, char **argv);
void cmd_benchmark(int argc, char **argv);

#endif // COMMANDS_H