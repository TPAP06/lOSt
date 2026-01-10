// kernel/shell/commands.c - Built-in commands implementation

#include "commands.h"
#include "../drivers/screen.h"
#include "../lib/string.h"
#include "../lib/io.h"
#include "../drivers/timer.h"
#include "../memory/pmm.h"
#include "../memory/heap.h"

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
    {"memtest", "Test memory allocation", cmd_memtest}
};

// Just use the macro, remove the const int
#define COMMAND_COUNT (sizeof(commands) / sizeof(command_t))
// Initialize commands
void commands_init(void)
{
    // Nothing to do yet
}

// Get all commands
const command_t *commands_get_all(int *count)
{
    if (count) {
        *count = COMMAND_COUNT;
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
    for (size_t i = 0; i < COMMAND_COUNT; i++) {  // Changed int to size_t
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
    
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
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
    screen_write_color("===================\n", COLOR_YELLOW, COLOR_BLACK);
    
    uint32_t total = pmm_get_total_memory();
    uint32_t used = pmm_get_used_memory();
    uint32_t free = pmm_get_free_memory();
    uint32_t free_pages = pmm_get_free_pages();
    
    char num_str[32];
    
    // Total memory
    screen_write("  Total Memory:  ");
    itoa(total, num_str, 10);
    screen_write_color(num_str, COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_write(" KB\n");
    
    // Used memory
    screen_write("  Used Memory:   ");
    itoa(used, num_str, 10);
    screen_write_color(num_str, COLOR_LIGHT_RED, COLOR_BLACK);
    screen_write(" KB\n");
    
    // Free memory
    screen_write("  Free Memory:   ");
    itoa(free, num_str, 10);
    screen_write_color(num_str, COLOR_LIGHT_GREEN, COLOR_BLACK);
    screen_write(" KB\n");
    
    // Free pages
    screen_write("  Free Pages:    ");
    itoa(free_pages, num_str, 10);
    screen_write_color(num_str, COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_write(" (4KB pages)\n");
    
    // Usage percentage
    if (total > 0) {
        uint32_t percent = (used * 100) / total;
        screen_write("  Usage:         ");
        itoa(percent, num_str, 10);
        
        if (percent < 50) {
            screen_write_color(num_str, COLOR_LIGHT_GREEN, COLOR_BLACK);
        } else if (percent < 80) {
            screen_write_color(num_str, COLOR_YELLOW, COLOR_BLACK);
        } else {
            screen_write_color(num_str, COLOR_LIGHT_RED, COLOR_BLACK);
        }
        screen_write("%\n");
    }
    
    screen_write("\n");
}

// Memory test command
// Memory test command with debugging
void cmd_memtest(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    screen_write_color("\nMemory Allocation Test:\n", COLOR_YELLOW, COLOR_BLACK);
    screen_write_color("=======================\n", COLOR_YELLOW, COLOR_BLACK);
    
    // Show memory status FIRST
    screen_write("Memory status before tests:\n");
    char num_str[32];
    
    uint32_t free_pages = pmm_get_free_pages();
    itoa(free_pages, num_str, 10);
    screen_write("  Free pages: ");
    screen_write(num_str);
    screen_write("\n");
    
    uint32_t free_kb = pmm_get_free_memory();
    itoa(free_kb, num_str, 10);
    screen_write("  Free memory: ");
    screen_write(num_str);
    screen_write(" KB\n\n");
    
    // ... rest of tests
    // Test 1: Simple allocation with malloc
    screen_write("Test 1: malloc(1024)... ");
    void *ptr1 = malloc(1024);
    if (ptr1) {
        screen_write_color("OK ", COLOR_LIGHT_GREEN, COLOR_BLACK);
        
        // Show address
        char addr_str[32];
        itoa((uint32_t)((uint64_t)ptr1 >> 32), addr_str, 16);
        screen_write("(addr: 0x");
        screen_write(addr_str);
        itoa((uint32_t)((uint64_t)ptr1 & 0xFFFFFFFF), addr_str, 16);
        screen_write(addr_str);
        screen_write(")\n");
        
        free(ptr1);
        screen_write("        Freed successfully\n");
    } else {
        screen_write_color("FAILED\n", COLOR_LIGHT_RED, COLOR_BLACK);
    }
    
    // Test 2: calloc - allocate and zero
    screen_write("Test 2: calloc(10, 512)... ");
    
    // First try malloc to see if allocation works
    void *test_malloc = malloc(5120);
    if (!test_malloc) {
        screen_write_color("FAILED - malloc failed\n", COLOR_LIGHT_RED, COLOR_BLACK);
        return;
    }
    free(test_malloc);
    screen_write_color("malloc OK, ", COLOR_LIGHT_GREEN, COLOR_BLACK);
    
    // Now try calloc
    void *ptr_calloc = calloc(10, 512);
    if (ptr_calloc) {
        screen_write_color("calloc OK\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
        
        // Verify it's zeroed (check first few bytes)
        unsigned char *bytes = (unsigned char*)ptr_calloc;
        bool is_zeroed = true;
        for (int i = 0; i < 10; i++) {
            if (bytes[i] != 0) {
                is_zeroed = false;
                break;
            }
        }
        
        if (is_zeroed) {
            screen_write("        Memory properly zeroed\n");
        } else {
            screen_write_color("        WARNING: Memory not zeroed!\n", COLOR_YELLOW, COLOR_BLACK);
        }
        
        free(ptr_calloc);
        screen_write("        Freed successfully\n");
    } else {
        screen_write_color("FAILED\n", COLOR_LIGHT_RED, COLOR_BLACK);
    }
    
    // Test 3: Multiple allocations
    screen_write("Test 3: Multiple malloc... ");
    void *ptrs[10];
    bool success = true;
    for (int i = 0; i < 10; i++) {
        ptrs[i] = malloc(512);
        if (ptrs[i] == NULL) {
            success = false;
            screen_write_color("FAILED at allocation ", COLOR_LIGHT_RED, COLOR_BLACK);
            char num[16];
            itoa(i, num, 10);
            screen_write(num);
            screen_write("\n");
            break;
        }
    }
    if (success) {
        screen_write_color("OK\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
        for (int i = 0; i < 10; i++) {
            free(ptrs[i]);
        }
        screen_write("        All freed successfully\n");
    }
    
    screen_write("\n");
    screen_write_color("Tests completed!\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
}