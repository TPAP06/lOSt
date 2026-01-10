// kernel/kernel.c - Main kernel with memory management

#include "drivers/screen.h"
#include "drivers/keyboard.h"
#include "drivers/timer.h"
#include "interrupts/idt.h"
#include "interrupts/isr.h"
#include "memory/pmm.h"      // ADD
#include "memory/heap.h"     // ADD
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
    
    // Read memory info from bootloader (stored at 0x9000)
    uint16_t *mem_info = (uint16_t*)0x9000;
    uint32_t mem_1mb_16mb = mem_info[0];
    uint32_t mem_above_16mb = mem_info[2];
    
    // Calculate total memory
    uint32_t total_memory_kb;
    
    if (mem_1mb_16mb == 0 && mem_above_16mb == 0) {
        // Detection failed - assume 32MB
        screen_write_color("Memory detection failed, assuming 32MB\n", COLOR_YELLOW, COLOR_BLACK);
        total_memory_kb = 32 * 1024;
    } else {
        total_memory_kb = 1024 + mem_1mb_16mb + (mem_above_16mb * 64);
    }
    
    char debug_str[32];
    screen_write("Detected memory: ");
    itoa(total_memory_kb, debug_str, 10);
    screen_write(debug_str);
    screen_write(" KB\n");
    
    // Initialize memory management
    pmm_init(total_memory_kb);
    heap_init();
    
    // Show PMM status
    uint32_t free_pages = pmm_get_free_pages();
    uint32_t free_kb = pmm_get_free_memory();
    itoa(free_pages, debug_str, 10);
    screen_write("PMM: ");
    screen_write(debug_str);
    screen_write(" free pages (");
    itoa(free_kb, debug_str, 10);
    screen_write(debug_str);
    screen_write(" KB)\n\n");
    
    // Delay to read output
    for (volatile int i = 0; i < 50000000; i++);
    
    // Initialize keyboard
    keyboard_init();
    
    // Initialize and run shell
    shell_init();
    shell_run();
    
    while (1) {
        __asm__ volatile ("hlt");
    }
}