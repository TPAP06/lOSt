// kernel/interrupts/isr.h

#ifndef ISR_H
#define ISR_H

#include <stdint.h>

typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed)) registers_t;

typedef void (*irq_handler_t)(registers_t *regs);

void irq_install_handler(int irq, irq_handler_t handler);
void irq_uninstall_handler(int irq);
void isr_init(void);

// Public EOI function for testing
void pic_send_eoi_public(uint8_t irq);

#endif