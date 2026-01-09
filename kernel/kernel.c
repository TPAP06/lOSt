// kernel/kernel.c - Main kernel with shell

#include "drivers/screen.h"
#include "drivers/keyboard.h"
#include "interrupts/idt.h"
#include "interrupts/isr.h"
#include "shell/shell.h"

void kernel_main(void)
{
    // Initialize screen
    screen_init();
    
    // Initialize interrupts
    isr_init();
    idt_init();
    __asm__ volatile ("sti");

    // Initialize timer
    timer_init();
    
    // Initialize keyboard
    keyboard_init();
    
    // Initialize and run shell
    shell_init();
    shell_run();
    
    // Should never reach here
    while (1) {
        __asm__ volatile ("hlt");
    }
}