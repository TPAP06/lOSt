// kernel/interrupts/idt.h - Interrupt Descriptor Table

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// IDT entry structure
typedef struct {
    uint16_t offset_low;    // Lower 16 bits of ISR address
    uint16_t selector;      // Code segment selector
    uint8_t ist;            // Interrupt Stack Table offset
    uint8_t type_attr;      // Type and attributes
    uint16_t offset_mid;    // Middle 16 bits of ISR address
    uint32_t offset_high;   // Upper 32 bits of ISR address
    uint32_t zero;          // Reserved
} __attribute__((packed)) idt_entry_t;

// IDT pointer structure
typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idt_ptr_t;

// Number of IDT entries
#define IDT_ENTRIES 256

// Initialize the IDT
void idt_init(void);

// Set an IDT gate
void idt_set_gate(uint8_t num, uint64_t handler, uint16_t selector, uint8_t flags);

#endif // IDT_H