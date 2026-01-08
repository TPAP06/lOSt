; kernel/kernel_entry.asm - Entry point for the kernel

[bits 64]
[extern kernel_main]

global _start

_start:
    ; Make sure we have a clean state
    cli
    
    ; Clear all segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up stack
    mov rsp, 0x90000
    
    ; Call the C kernel
    call kernel_main
    
    ; If kernel_main returns, halt
.hang:
    cli
    hlt
    jmp .hang