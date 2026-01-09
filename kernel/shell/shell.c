// kernel/shell/shell.c - Shell with fixed history navigation

#include "shell.h"
#include "commands.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../lib/string.h"

// Shell state
static char input_buffer[SHELL_INPUT_MAX];

// Command history
static char history[SHELL_HISTORY_SIZE][SHELL_INPUT_MAX];
static int history_count = 0;  // Total number of commands stored
static int history_write_pos = 0;  // Where to write next command

// Initialize shell
void shell_init(void)
{
    // Clear history
    for (int i = 0; i < SHELL_HISTORY_SIZE; i++) {
        history[i][0] = '\0';
    }
    history_count = 0;
    history_write_pos = 0;
    
    commands_init();
    
    // Print welcome banner
    screen_write_color("================================\n", COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_write_color("       Welcome to MyOS          \n", COLOR_YELLOW, COLOR_BLACK);
    screen_write_color("================================\n", COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_write("\n");
    
    screen_write("Type 'help' for a list of commands.\n");
    screen_write("Use UP/DOWN arrows to navigate command history.\n");
    screen_write("\n");
}

// Add command to history
void shell_add_to_history(const char *cmd)
{
    // Don't add empty commands
    if (strlen(cmd) == 0) {
        return;
    }
    
    // Don't add if same as last command
    if (history_count > 0) {
        int last_idx = (history_write_pos - 1 + SHELL_HISTORY_SIZE) % SHELL_HISTORY_SIZE;
        if (strcmp(history[last_idx], cmd) == 0) {
            return;
        }
    }
    
    // Add to history
    strncpy(history[history_write_pos], cmd, SHELL_INPUT_MAX - 1);
    history[history_write_pos][SHELL_INPUT_MAX - 1] = '\0';
    
    history_write_pos = (history_write_pos + 1) % SHELL_HISTORY_SIZE;
    
    if (history_count < SHELL_HISTORY_SIZE) {
        history_count++;
    }
}

// Print shell prompt
void shell_print_prompt(void)
{
    screen_write_color(SHELL_PROMPT, SHELL_COLOR_PROMPT, COLOR_BLACK);
}

// Execute a command
void shell_execute(const char *input)
{
    // Skip empty input
    if (strlen(input) == 0) {
        return;
    }
    
    // Add to history
    shell_add_to_history(input);
    
    // Parse command line
    char input_copy[SHELL_INPUT_MAX];
    strncpy(input_copy, input, SHELL_INPUT_MAX - 1);
    input_copy[SHELL_INPUT_MAX - 1] = '\0';
    
    char *argv[32];
    int argc = commands_parse(input_copy, argv, 32);
    
    if (argc == 0) {
        return;
    }
    
    // Check for special shell commands
    if (strcmp(argv[0], "history") == 0) {
        // Show command history
        screen_write_color("\nCommand History:\n", COLOR_YELLOW, COLOR_BLACK);
        screen_write_color("----------------\n", COLOR_YELLOW, COLOR_BLACK);
        
        if (history_count == 0) {
            screen_write("  (empty)\n");
        } else {
            int start = (history_write_pos - history_count + SHELL_HISTORY_SIZE) % SHELL_HISTORY_SIZE;
            for (int i = 0; i < history_count; i++) {
                int idx = (start + i) % SHELL_HISTORY_SIZE;
                
                // Print line number
                char num_str[16];
                itoa(i + 1, num_str, 10);
                screen_write("  ");
                screen_write_color(num_str, COLOR_LIGHT_CYAN, COLOR_BLACK);
                screen_write(". ");
                screen_write(history[idx]);
                screen_write("\n");
            }
        }
        screen_write("\n");
        return;
    }
    
    // Execute command
    if (!commands_execute(argv[0], argc, argv)) {
        screen_write_color("Unknown command: ", SHELL_COLOR_ERROR, COLOR_BLACK);
        screen_write(argv[0]);
        screen_write("\n");
        screen_write("Type 'help' for a list of commands.\n");
    }
}

// Main shell loop
// Main shell loop
void shell_run(void)
{
    while (1) {
        shell_print_prompt();
        
        // Read input with history support
        keyboard_readline_history(input_buffer, SHELL_INPUT_MAX,
                                  history, SHELL_HISTORY_SIZE, history_count, &history_write_pos);
        
        shell_execute(input_buffer);
    }
}