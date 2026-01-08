// kernel/kernel.c - Full interactive kernel

#include "drivers/screen.h"
#include "drivers/keyboard.h"
#include "interrupts/idt.h"
#include "interrupts/isr.h"
#include "lib/string.h"

void kernel_main(void)
{
    // Initialize screen
    screen_init();
    
    // Print welcome banner
    screen_write_color("================================\n", COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_write_color("    Welcome to MyOS v0.3        \n", COLOR_YELLOW, COLOR_BLACK);
    screen_write_color("================================\n", COLOR_LIGHT_CYAN, COLOR_BLACK);
    screen_write("\n");
    
    screen_write_color("Initializing system...\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
    
    // Initialize interrupts
    screen_write("  - Setting up interrupts... ");
    isr_init();
    idt_init();
    __asm__ volatile ("sti");
    screen_write_color("OK\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
    
    // Initialize keyboard
    screen_write("  - Initializing keyboard... ");
    keyboard_init();
    screen_write_color("OK\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
    
    screen_write("\n");
    screen_write_color("System ready!\n", COLOR_LIGHT_GREEN, COLOR_BLACK);
    screen_write("\n");
    screen_write("Type something and press Enter:\n");
    screen_write("(Try 'help', 'clear', or 'info')\n\n");
    
    // Main input loop
    char input_buffer[256];
    
    while (1) {
        screen_write_color("> ", COLOR_LIGHT_CYAN, COLOR_BLACK);
        keyboard_readline(input_buffer, sizeof(input_buffer));
        
        // Process commands
        if (strlen(input_buffer) == 0) {
            continue;
        }
        
        if (strcmp(input_buffer, "help") == 0) {
            screen_write_color("\nAvailable commands:\n", COLOR_YELLOW, COLOR_BLACK);
            screen_write("  help  - Show this help message\n");
            screen_write("  clear - Clear the screen\n");
            screen_write("  info  - Show system information\n");
            screen_write("  echo  - Echo back your input\n");
            screen_write("\n");
        }
        else if (strcmp(input_buffer, "clear") == 0) {
            screen_clear();
            screen_write_color("MyOS v0.3\n\n", COLOR_YELLOW, COLOR_BLACK);
        }
        else if (strcmp(input_buffer, "info") == 0) {
            screen_write_color("\nSystem Information:\n", COLOR_YELLOW, COLOR_BLACK);
            screen_write("  OS Name: MyOS\n");
            screen_write("  Version: 0.3\n");
            screen_write("  Architecture: x86_64\n");
            screen_write("  Mode: Long mode (64-bit)\n");
            screen_write("\n");
        }
        else if (strncmp(input_buffer, "echo ", 5) == 0) {
            screen_write(&input_buffer[5]);
            screen_write("\n");
        }
        else {
            screen_write_color("Unknown command: ", COLOR_LIGHT_RED, COLOR_BLACK);
            screen_write(input_buffer);
            screen_write("\n");
            screen_write("Type 'help' for available commands.\n");
        }
    }
}