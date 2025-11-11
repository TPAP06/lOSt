[org 0x1000]
[bits 16]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax

    mov si, msg_real
    call print_real      ; BIOS print in real mode

    ; ----------------------------
    ; Enter protected mode
    ; ----------------------------
    lgdt [gdt_descriptor]     ; Load GDT

    mov eax, cr0
    or eax, 1                 ; set PE bit
    mov cr0, eax
    jmp 0x08:protected_mode_entry   ; far jump to code selector

; ----------------------------
; BIOS print (16-bit)
; ----------------------------
print_real:
    mov ah, 0x0E
.next_real:
    lodsb
    test al, al
    jz .done_real
    int 0x10
    jmp .next_real
.done_real:
    ret

; ----------------------------
; GDT setup
; ----------------------------
align 4
gdt_start:
    dq 0x0000000000000000   ; null
    dq 0x00CF9A000000FFFF   ; code: base=0, limit=4GB, 32-bit
    dq 0x00CF92000000FFFF   ; data: base=0, limit=4GB, 32-bit
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; ----------------------------
; Protected mode code
; ----------------------------
[bits 32]
protected_mode_entry:
    ; ----------------------------
    ; Setup data segments & stack
    ; ----------------------------
    mov ax, 0x10       ; data selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, stack_top ; initialize 32-bit stack

    ; ----------------------------
    ; Print message directly to VGA
    ; ----------------------------
    mov esi, msg_pm
    mov edi, 0xB8000
    call print_pm

.hang:
    hlt
    jmp .hang

; ----------------------------
; VGA print routine (32-bit)
; ----------------------------
print_pm:
.next_pm:
    mov al, [esi]
    test al, al
    jz .done_pm
    mov [edi], al
    mov byte [edi + 1], 0x07
    add esi, 1
    add edi, 2
    jmp .next_pm
.done_pm:
    ret

; ----------------------------
; Data & stack (32-bit)
; ----------------------------
[bits 16]
msg_real db "Entering protected mode...", 0

[bits 32]
msg_pm db "Hello from 32-bit protected mode!", 0

align 16
stack_bottom:
    times 4096 db 0        ; 4 KB stack
stack_top:
