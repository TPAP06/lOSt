; boot/boot32.asm — Stage 2: transition from real mode → protected mode → long mode
[org 0x1000]
[bits 16]

start16:
    ; We're still in real mode, loaded at 0x0000:0x1000
    mov si, msg_stage2
    call print_string_16

    ; Enable A20 line (allows access to memory above 1MB)
    call enable_a20

    ; Load GDT
    lgdt [gdt_descriptor]

    ; Enter protected mode
    mov eax, cr0
    or eax, 1               ; set PE (Protection Enable) bit
    mov cr0, eax

    ; Far jump to flush pipeline and load CS with 32-bit code segment
    jmp CODE_SEG:start32

; --- 16-bit helper functions ---
print_string_16:
    pusha
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    popa
    ret

enable_a20:
    ; Fast A20 gate method (works on most modern systems)
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

msg_stage2: db "Stage 2 loaded, entering protected mode...", 13, 10, 0

; --- GDT (Global Descriptor Table) ---
align 8
gdt_start:
    ; Null descriptor (required)
    dq 0

gdt_code_32:
    ; 32-bit code segment
    dw 0xFFFF       ; limit low
    dw 0            ; base low
    db 0            ; base middle
    db 10011010b    ; access: present, ring 0, code, executable, readable
    db 11001111b    ; flags + limit high: 4K granularity, 32-bit
    db 0            ; base high

gdt_data_32:
    ; 32-bit data segment
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b    ; access: present, ring 0, data, writable
    db 11001111b
    db 0

gdt_code_64:
    ; 64-bit code segment
    dw 0xFFFF
    dw 0
    db 0
    db 10011010b    ; access: present, ring 0, code, executable, readable
    db 10101111b    ; flags: 4K granularity, 64-bit (L bit set)
    db 0

gdt_data_64:
    ; 64-bit data segment
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b
    db 10101111b
    db 0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; size
    dd gdt_start                 ; address

; Segment selector offsets
CODE_SEG equ gdt_code_32 - gdt_start
DATA_SEG equ gdt_data_32 - gdt_start
CODE_SEG_64 equ gdt_code_64 - gdt_start
DATA_SEG_64 equ gdt_data_64 - gdt_start

; ============================================================================
; 32-BIT PROTECTED MODE
; ============================================================================
[bits 32]
start32:
    ; Load all segment registers with 32-bit data segment
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000        ; set up stack in free memory

    ; Print message to screen (VGA text mode)
    mov esi, msg_pmode
    call print_string_32

    ; COPY KERNEL from 0x10000 to 0x100000 (1MB)
    mov esi, 0x10000        ; source
    mov edi, 0x100000       ; destination
    mov ecx, 51200          ; 100 sectors * 512 bytes = 51200 bytes
    rep movsb               ; copy byte by byte

    ; Check if CPU supports long mode
    call check_long_mode
    test eax, eax
    jz .no_long_mode

    ; Set up paging for long mode
    call setup_paging

    ; Enable PAE (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5          ; set PAE bit
    mov cr4, eax

    ; Load PML4 address into CR3
    mov eax, PML4
    mov cr3, eax

    ; Enable long mode in EFER MSR
    mov ecx, 0xC0000080     ; EFER MSR
    rdmsr
    or eax, 1 << 8          ; set LM (Long Mode) bit
    wrmsr

    ; Enable paging (this activates long mode)
    mov eax, cr0
    or eax, 1 << 31         ; set PG (Paging) bit
    mov cr0, eax

    ; Load 64-bit GDT
    lgdt [gdt_descriptor]

    ; Far jump to 64-bit code
    jmp CODE_SEG_64:start64

.no_long_mode:
    mov esi, msg_no_64
    call print_string_32
    cli
    hlt

; --- Check if CPU supports long mode ---
check_long_mode:
    ; Check for CPUID support
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    xor eax, ecx
    jz .no_cpuid

    ; Check for extended CPUID
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode

    ; Check for long mode
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode

    mov eax, 1
    ret

.no_cpuid:
.no_long_mode:
    xor eax, eax
    ret

; --- Set up identity paging for first 4MB ---
setup_paging:
    ; Clear page tables
    mov edi, PML4
    mov ecx, 4096 * 3 / 4   ; clear 3 pages (PML4, PDPT, PDT)
    xor eax, eax
    rep stosd

    ; PML4[0] → PDPT
    mov eax, PDPT
    or eax, 3               ; present + writable
    mov [PML4], eax

    ; PDPT[0] → PDT
    mov eax, PDT
    or eax, 3
    mov [PDPT], eax

    ; PDT[0] → first 2MB huge page (covers 0x000000-0x200000)
    mov eax, 0
    or eax, 0b10000011      ; present + writable + huge page (2MB)
    mov [PDT], eax

    ret

; --- Print string in 32-bit mode (VGA text buffer at 0xB8000) ---
print_string_32:
    pusha
    mov edi, 0xB8000 + 160 * 2  ; row 2
.loop:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0F            ; white on black
    stosw
    jmp .loop
.done:
    popa
    ret

msg_pmode: db "Protected mode active, entering long mode...", 0
msg_no_64: db "ERROR: CPU does not support 64-bit long mode!", 0

; ============================================================================
; 64-BIT LONG MODE
; ============================================================================
[bits 64]
start64:
    ; Clear segment registers (not used in long mode except FS/GS)
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set up 64-bit stack
    mov rsp, 0x90000

    ; Print success message
    mov rsi, msg_64bit
    call print_string_64

    ; Jump to kernel at 0x100000 (USE JMP NOT CALL!)
    jmp 0x100000

; --- Print string in 64-bit mode ---
print_string_64:
    push rax
    push rdi
    mov rdi, 0xB8000 + 160 * 4  ; row 4
.loop:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0A            ; light green on black
    stosw
    jmp .loop
.done:
    pop rdi
    pop rax
    ret

msg_64bit: db "64-bit long mode active! Jumping to kernel...", 0

; --- Page tables (must be 4KB aligned) ---
align 4096
PML4:
    times 4096 db 0
PDPT:
    times 4096 db 0
PDT:
    times 4096 db 0