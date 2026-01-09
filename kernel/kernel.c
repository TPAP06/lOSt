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
    uint32_t mem_1mb_16mb = mem_info[0];  // KB between 1-16MB
    uint32_t mem_above_16mb = mem_info[2]; // 64KB blocks above 16MB
    
    // Calculate total memory in KB
    uint32_t total_memory_kb = 1024 + mem_1mb_16mb + (mem_above_16mb * 64);
    
    // Initialize memory management
    pmm_init(total_memory_kb);
    heap_init();
    
    // Initialize keyboard
    keyboard_init();
    
    // Initialize and run shell
    shell_init();
    shell_run();
    
    while (1) {
        __asm__ volatile ("hlt");
    }
}