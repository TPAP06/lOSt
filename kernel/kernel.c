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
    // Initialize screen (early)
    screen_init();
    
    // Initialize interrupts
    isr_init();
    idt_init();
    __asm__ volatile ("sti");
    
    // Initialize timer
    timer_init();
    
    // Memory detection and initialization
    uint16_t *mem_info = (uint16_t*)0x9000;
    uint32_t mem_1mb_16mb = mem_info[0];
    uint32_t mem_above_16mb = mem_info[2];
    uint32_t total_memory_kb;
    
    if (mem_1mb_16mb == 0 && mem_above_16mb == 0) {
        total_memory_kb = 32 * 1024;
    } else {
        total_memory_kb = 1024 + mem_1mb_16mb + (mem_above_16mb * 64);
    }
    
    pmm_init(total_memory_kb);
    heap_init();
    
    // NOW initialize scrollback (after heap is ready)
    screen_init_scrollback();  // ADD THIS
    
    // Initialize filesystem
    // vfs_init();  // TODO: Implement VFS
    
    // Initialize keyboard
    keyboard_init();
    
    // Initialize and run shell
    shell_init();
    shell_run();
    
    while (1) {
        __asm__ volatile ("hlt");
    }
}