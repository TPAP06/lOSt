// kernel/interrupts/isr.c - Ultra minimal for debugging

#include "isr.h"
#include "../lib/io.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

static irq_handler_t irq_handlers[16] = {0};

// Remap PIC
static void pic_remap(void)
{
    outb(PIC1_COMMAND, 0x11);
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();
    
    outb(PIC1_DATA, 32);
    io_wait();
    outb(PIC2_DATA, 40);
    io_wait();
    
    outb(PIC1_DATA, 0x04);
    io_wait();
    outb(PIC2_DATA, 0x02);
    io_wait();
    
    outb(PIC1_DATA, 0x01);
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();
    
    // Enable IRQ0 (timer) and IRQ1 (keyboard)
    // 0xFC = binary 11111100 = both IRQ0 and IRQ1 enabled
    outb(PIC1_DATA, 0xFC);
    outb(PIC2_DATA, 0xFF);
}

// ISR handler - MINIMAL (just halt)
// ISR handler - show exception info
void isr_handler(registers_t *regs)
{
    // Write to VGA memory
    volatile uint16_t *vga = (volatile uint16_t *)0xB8000;
    vga[0] = 'E' | 0x4F00;  // White on red
    vga[1] = 'X' | 0x4F00;
    vga[2] = 'C' | 0x4F00;
    
    // Show exception number
    uint8_t num = regs->int_no;
    vga[3] = ' ' | 0x4F00;
    vga[4] = ((num / 10) + '0') | 0x4F00;
    vga[5] = ((num % 10) + '0') | 0x4F00;
    
    // Show error code if present
    vga[7] = 'E' | 0x4F00;
    vga[8] = 'R' | 0x4F00;
    vga[9] = 'R' | 0x4F00;
    vga[10] = ':' | 0x4F00;
    
    uint32_t err = regs->err_code;
    for (int i = 0; i < 8; i++) {
        uint8_t nibble = (err >> (28 - i*4)) & 0xF;
        char c = nibble < 10 ? '0' + nibble : 'A' + nibble - 10;
        vga[11 + i] = c | 0x4F00;
    }
    
    // Halt
    while(1) {
        __asm__ volatile("cli; hlt");
    }
}

// IRQ handler - ABSOLUTELY MINIMAL
void irq_handler(registers_t *regs)
{
    // Calculate IRQ number
    int irq = regs->int_no - 32;
    
    // Call custom handler if exists
    if (irq >= 0 && irq < 16 && irq_handlers[irq] != 0) {
        irq_handlers[irq](regs);
    }
    
    // Send EOI
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

// Public EOI
void pic_send_eoi_public(uint8_t irq)
{
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

// Install handler
void irq_install_handler(int irq, irq_handler_t handler)
{
    if (irq >= 0 && irq < 16) {
        irq_handlers[irq] = handler;
    }
}

// Uninstall
void irq_uninstall_handler(int irq)
{
    if (irq >= 0 && irq < 16) {
        irq_handlers[irq] = 0;
    }
}

// Initialize
void isr_init(void)
{
    pic_remap();
    
    for (int i = 0; i < 16; i++) {
        irq_handlers[i] = 0;
    }
}