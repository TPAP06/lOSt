// kernel/shell/commands.c - Built-in commands implementation

#include "commands.h"
#include "../drivers/screen.h"
#include "../lib/string.h"
#include "../lib/io.h"
#include "../drivers/timer.h"

// Command registry
static command_t commands[] = {
    {"help", "Show available commands", cmd_help},
    {"clear", "Clear the screen", cmd_clear},
    {"echo", "Echo text to the screen", cmd_echo},
    {"info", "Display system information", cmd_info},
    {"uptime", "Show system uptime", cmd_uptime},
    {"date", "Show current time since boot", cmd_date},
    {"sleep", "Sleep for N seconds", cmd_sleep},
    {"benchmark", "Run a simple benchmark", cmd_benchmark},
    {"reboot", "Reboot the system", cmd_reboot},
    {"shutdown", "Shutdown the system", cmd_shutdown},
    {"calc", "Simple calculator (add, sub, mul, div)", cmd_calc},
    {"color", "Change text color", cmd_color},
    {"meminfo", "Display memory information", cmd_meminfo},
};

static const int command_count = sizeof(commands) / sizeof(command_t);

// Initialize commands
void commands_init(void)
{
    // Nothing to do yet
}

// Get all commands
const command_t *commands_get_all(int *count)
{
    if (count) {
        *count = command_count;
    }
    return commands;
}

// Parse command line into argc/argv
int commands_parse(char *input, char **argv, int max_args)
{
    int argc = 0;
    char *ptr = input;
    bool in_word = false;
    
    while (*ptr && argc < max_args) {
        if (*ptr == ' ' || *ptr == '\t') {
            if (in_word) {
                *ptr = '\0';
                in_word = false;
            }
        } else {
            if (!in_word) {
                argv[argc++] = ptr;
                in_word = true;
            }
        }
        ptr++;
    }
    
    return argc;
}

// Execute a command
bool commands_execute(const char *name, int argc, char **argv)
{
    for (int i = 0; i < command_count; i++) {
        if (strcmp(name, commands[i].name) == 0) {
            commands[i].handler(argc, argv);
            return true;
        }
    }
    return false;
}

// ============================================================================
// COMMAND IMPLEMENTATIONS
// ============================================================================

// Help command
// Help command (updated)
// Help command (UPDATED)
// Help command (SIMPLE VERSION)
void cmd_help(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    screen_write_color("\n====================================\n", COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_write_color("         MyOS Command Help          \n", COLOR_YELLOW, COLOR_BLACK);
    screen_write_color("====================================\n", COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_write("\n");
    
    screen_write_color("Available Commands:\n", COLOR_YELLOW, COLOR_BLACK);
    screen_write_color("-------------------\n", COLOR_YELLOW, COLOR_BLACK);
    
    for (int i = 0; i < command_count; i++) {
        screen_write("  ");
        screen_write_color(commands[i].name, COLOR_LIGHT_CYAN, COLOR_BLACK);
        
        // Pad to align descriptions
        int padding = 12 - strlen(commands[i].name);
        for (int j = 0; j < padding; j++) {
            screen_write(" ");
        }
        
        screen_write("- ");
        screen_write(commands[i].description);
        screen_write("\n");
    }
    
    // Special shell commands
    screen_write("\n");
    screen_write_color("Shell Commands:\n", COLOR_YELLOW, COLOR_BLACK);
    screen_write("  ");
    screen_write_color("history", COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_write("     - Show command history\n");
    
    screen_write("\n");
    screen_write_color("Keyboard Shortcuts:\n", COLOR_YELLOW, COLOR_BLACK);
    screen_write("  UP/DOWN arrows - Navigate command history\n");
    screen_write("  BACKSPACE      - Delete character\n");
    
    screen_write("\n");
    screen_write_color("Usage Examples:\n", COLOR_YELLOW, COLOR_BLACK);
    screen_write("  uptime           - Show how long system has been running\n");
    screen_write("  sleep 3          - Wait for 3 seconds\n");
    screen_write("  calc 10 add 5    - Simple calculator\n");
    screen_write("  echo Hello!      - Print text\n");
    screen_write("  benchmark        - Run performance test\n");
    
    screen_write("\n");
}

// Clear command
void cmd_clear(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    screen_clear();
    screen_write_color("MyOS Shell\n\n", COLOR_YELLOW, COLOR_BLACK);
}

// Echo command
void cmd_echo(int argc, char **argv)
{
    if (argc < 2) {
        screen_write("Usage: echo <text>\n");
        return;
    }
    
    for (int i = 1; i < argc; i++) {
        screen_write(argv[i]);
        if (i < argc - 1) {
            screen_write(" ");
        }
    }
    screen_write("\n");
}

// Info command
void cmd_info(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    screen_write_color("\nSystem Information:\n", COLOR_YELLOW, COLOR_BLACK);
    screen_write_color("-------------------\n", COLOR_YELLOW, COLOR_BLACK);
    screen_write("  OS Name:      MyOS\n");
    screen_write("  Version:      0.4\n");
    screen_write("  Architecture: x86_64\n");
    screen_write("  CPU Mode:     Long Mode (64-bit)\n");
    screen_write("  Author:       Your Name\n");
    screen_write("\n");
}

// Uptime command (placeholder - needs timer)
void cmd_uptime(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    char uptime_str[64];
    timer_format_uptime(uptime_str, sizeof(uptime_str));
    
    screen_write("System uptime: ");
    screen_write_color(uptime_str, COLOR_LIGHT_GREEN, COLOR_BLACK);
    screen_write("\n");
    
    // Also show in milliseconds
    char ms_str[32];
    uint64_t ms = timer_get_uptime_ms();
    
    // Convert uint64_t to string manually (since itoa only does int)
    if (ms < 1000000) {
        itoa((int)ms, ms_str, 10);
        screen_write("  (");
        screen_write(ms_str);
        screen_write(" milliseconds)\n");
    }
}

void cmd_sleep(int argc, char **argv)
{
    if (argc < 2) {
        screen_write("Usage: sleep <seconds>\n");
        screen_write("Example: sleep 3\n");
        return;
    }
    
    int seconds = atoi(argv[1]);
    
    if (seconds <= 0 || seconds > 60) {
        screen_write_color("Error: ", COLOR_LIGHT_RED, COLOR_BLACK);
        screen_write("Please specify a number between 1 and 60 seconds.\n");
        return;
    }
    
    char sec_str[16];
    itoa(seconds, sec_str, 10);
    
    screen_write("Sleeping for ");
    screen_write(sec_str);
    screen_write(" seconds...");
    
    timer_sleep(seconds);
    
    screen_write_color(" Done!\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
}
// Benchmark command
void cmd_benchmark(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    screen_write_color("\nRunning benchmark...\n", COLOR_YELLOW, COLOR_BLACK);
    
    // Test 1: Simple loop
    screen_write("Test 1: Integer arithmetic... ");
    uint64_t start = timer_get_uptime_ms();
    
    volatile int sum = 0;
    for (volatile int i = 0; i < 1000000; i++) {
        sum += i;
    }
    
    uint64_t elapsed = timer_get_uptime_ms() - start;
    char time_str[32];
    itoa((int)elapsed, time_str, 10);
    screen_write_color(time_str, COLOR_LIGHT_GREEN, COLOR_BLACK);
    screen_write(" ms\n");
    
    // Test 2: String operations
    screen_write("Test 2: String operations... ");
    start = timer_get_uptime_ms();
    
    char buffer[256];
    for (int i = 0; i < 1000; i++) {
        strcpy(buffer, "Hello, World!");
        strlen(buffer);
    }
    
    elapsed = timer_get_uptime_ms() - start;
    itoa((int)elapsed, time_str, 10);
    screen_write_color(time_str, COLOR_LIGHT_GREEN, COLOR_BLACK);
    screen_write(" ms\n");
    
    // Test 3: Memory operations
    screen_write("Test 3: Memory operations... ");
    start = timer_get_uptime_ms();
    
    char large_buffer[1024];
    for (int i = 0; i < 100; i++) {
        memset(large_buffer, 0, sizeof(large_buffer));
    }
    
    elapsed = timer_get_uptime_ms() - start;
    itoa((int)elapsed, time_str, 10);
    screen_write_color(time_str, COLOR_LIGHT_GREEN, COLOR_BLACK);
    screen_write(" ms\n");
    
    screen_write_color("\nBenchmark complete!\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
}

// Date command (shows time since boot)
void cmd_date(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    uint64_t seconds = timer_get_uptime_seconds();
    char sec_str[32];
    
    if (seconds < 100000) {
        itoa((int)seconds, sec_str, 10);
        screen_write("Time since boot: ");
        screen_write_color(sec_str, COLOR_YELLOW, COLOR_BLACK);
        screen_write(" seconds\n");
    } else {
        screen_write("Time since boot: A very long time!\n");
    }
}

// Reboot command
void cmd_reboot(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    screen_write_color("Rebooting system...\n", COLOR_YELLOW, COLOR_BLACK);
    
    // Wait a moment
    for (volatile int i = 0; i < 10000000; i++);
    
    // Use keyboard controller to reboot
    uint8_t temp;
    do {
        temp = inb(0x64);
        if (temp & 0x01) {
            inb(0x60);
        }
    } while (temp & 0x02);
    
    outb(0x64, 0xFE);  // Send reboot command
    
    // If that didn't work, triple fault
    __asm__ volatile ("cli; hlt");
}

// Shutdown command
void cmd_shutdown(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    screen_clear();
    screen_write_color("\n\n\n", COLOR_WHITE, COLOR_BLACK);
    screen_write_color("          System Shutdown          \n", COLOR_YELLOW, COLOR_BLACK);
    screen_write_color("                                    \n", COLOR_WHITE, COLOR_BLACK);
    screen_write_color("   It is now safe to turn off      \n", COLOR_LIGHT_GREY, COLOR_BLACK);
    screen_write_color("        your computer.              \n", COLOR_LIGHT_GREY, COLOR_BLACK);
    
    // Halt CPU
    __asm__ volatile ("cli; hlt");
}

// Calculator command
void cmd_calc(int argc, char **argv)
{
    if (argc != 4) {
        screen_write("Usage: calc <num1> <op> <num2>\n");
        screen_write("Operations: add, sub, mul, div\n");
        screen_write("Example: calc 10 add 5\n");
        return;
    }
    
    int num1 = atoi(argv[1]);
    int num2 = atoi(argv[3]);
    int result = 0;
    bool valid = true;
    
    if (strcmp(argv[2], "add") == 0) {
        result = num1 + num2;
    } else if (strcmp(argv[2], "sub") == 0) {
        result = num1 - num2;
    } else if (strcmp(argv[2], "mul") == 0) {
        result = num1 * num2;
    } else if (strcmp(argv[2], "div") == 0) {
        if (num2 == 0) {
            screen_write_color("Error: Division by zero!\n", COLOR_LIGHT_RED, COLOR_BLACK);
            return;
        }
        result = num1 / num2;
    } else {
        screen_write_color("Unknown operation: ", COLOR_LIGHT_RED, COLOR_BLACK);
        screen_write(argv[2]);
        screen_write("\n");
        valid = false;
    }
    
    if (valid) {
        char result_str[32];
        itoa(result, result_str, 10);
        
        screen_write("Result: ");
        screen_write_color(result_str, COLOR_LIGHT_GREEN, COLOR_BLACK);
        screen_write("\n");
    }
}

// Color command
void cmd_color(int argc, char **argv)
{
    if (argc < 2) {
        screen_write("Usage: color <colorname>\n");
        screen_write("Available colors:\n");
        screen_write("  red, green, blue, yellow, cyan, magenta, white, grey\n");
        return;
    }
    
    vga_color fg = COLOR_WHITE;
    
    if (strcmp(argv[1], "red") == 0) {
        fg = COLOR_LIGHT_RED;
    } else if (strcmp(argv[1], "green") == 0) {
        fg = COLOR_LIGHT_GREEN;
    } else if (strcmp(argv[1], "blue") == 0) {
        fg = COLOR_LIGHT_BLUE;
    } else if (strcmp(argv[1], "yellow") == 0) {
        fg = COLOR_YELLOW;
    } else if (strcmp(argv[1], "cyan") == 0) {
        fg = COLOR_LIGHT_CYAN;
    } else if (strcmp(argv[1], "magenta") == 0) {
        fg = COLOR_LIGHT_MAGENTA;
    } else if (strcmp(argv[1], "white") == 0) {
        fg = COLOR_WHITE;
    } else if (strcmp(argv[1], "grey") == 0) {
        fg = COLOR_LIGHT_GREY;
    } else {
        screen_write_color("Unknown color: ", COLOR_LIGHT_RED, COLOR_BLACK);
        screen_write(argv[1]);
        screen_write("\n");
        return;
    }
    
    screen_set_color(fg, COLOR_BLACK);
    screen_write("Text color changed to ");
    screen_write(argv[1]);
    screen_write("\n");
}

// Memory info command (placeholder)
void cmd_meminfo(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    screen_write_color("\nMemory Information:\n", COLOR_YELLOW, COLOR_BLACK);
    screen_write_color("-------------------\n", COLOR_YELLOW, COLOR_BLACK);
    screen_write("  Total Memory:  ");
    screen_write_color("Not yet implemented\n", COLOR_YELLOW, COLOR_BLACK);
    screen_write("  Free Memory:   ");
    screen_write_color("Not yet implemented\n", COLOR_YELLOW, COLOR_BLACK);
    screen_write("  Used Memory:   ");
    screen_write_color("Not yet implemented\n", COLOR_YELLOW, COLOR_BLACK);
    screen_write("\n");
    screen_write("(Memory management will be added in Phase 4)\n");
}

