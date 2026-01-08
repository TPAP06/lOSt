; kernel/interrupts/isr.asm - Minimal working version

[bits 64]

extern isr_handler
extern irq_handler

; Macro for ISRs without error code
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push qword 0
    push qword %1
    jmp isr_common
%endmacro

; Macro for ISRs with error code
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push qword %1
    jmp isr_common
%endmacro

; Macro for IRQs
%macro IRQ 2
global irq%1
irq%1:
    push qword 0
    push qword %2
    jmp irq_common
%endmacro

; Define all ISRs (0-31)
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE   30
ISR_NOERRCODE 31

; Define all IRQs (32-47)
IRQ 0,  32
IRQ 1,  33
IRQ 2,  34
IRQ 3,  35
IRQ 4,  36
IRQ 5,  37
IRQ 6,  38
IRQ 7,  39
IRQ 8,  40
IRQ 9,  41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

; Common ISR stub
isr_common:
    ; Save ALL registers
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ; Pass pointer to stack frame as first argument
    mov rdi, rsp
    
    ; Save stack pointer
    mov rbp, rsp
    
    ; Align stack to 16 bytes (required for System V ABI)
    and rsp, ~0xF
    
    ; Call C handler
    call isr_handler
    
    ; Restore stack
    mov rsp, rbp
    
    ; Restore ALL registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    ; Remove error code and int number from stack
    add rsp, 16
    
    ; Return from interrupt
    iretq

; Common IRQ stub
irq_common:
    ; Save ALL registers (same as ISR)
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ; Pass pointer to stack frame
    mov rdi, rsp
    
    ; Save and align stack
    mov rbp, rsp
    and rsp, ~0xF
    
    ; Call C handler
    call irq_handler
    
    ; Restore stack
    mov rsp, rbp
    
    ; Restore ALL registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    ; Remove error code and int number
    add rsp, 16
    
    ; Return from interrupt
    iretq