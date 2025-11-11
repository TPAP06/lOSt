; stage2.asm — Loaded at 0x1000, enters 32-bit then 64-bit mode
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
; GDT setup (for 32-bit)
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
; Protected mode code (32-bit)
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
; ----------------------------
; Check for long mode support
; ----------------------------
mov eax, 0x80000000
cpuid
cmp eax, 0x80000001
jb no_long_mode
mov eax, 0x80000001
cpuid
test edx, (1 << 29)     ; long mode bit
jz no_long_mode
; ----------------------------
; Setup page tables for long mode (identity map first 2MB as example)
; ----------------------------
; Fill page tables (assuming labels resolve to correct addresses)
mov dword [pml4], pdpt + 0x3    ; PML4[0] -> PDPT (present + writable)
mov dword [pdpt], pd + 0x3      ; PDPT[0] -> PD (present + writable)
mov dword [pd], 0x00000083      ; PD[0] -> 2MB page at 0x0 (present + writable + huge page)
; Enable PAE
mov eax, cr4
or eax, (1 << 5)
mov cr4, eax
; Load PML4 into CR3
mov eax, pml4
mov cr3, eax
; Enable long mode in EFER
mov ecx, 0xC0000080
rdmsr
or eax, (1 << 8)
wrmsr
; Enable paging
mov eax, cr0
or eax, (1 << 31)
mov cr0, eax
; Now in 32-bit compatibility mode
; ----------------------------
; Load 64-bit GDT and jump to long mode
; ----------------------------
lgdt [gdt64_descriptor]
jmp 0x08:long_mode_entry  ; far jump to 64-bit code selector
no_long_mode:
mov esi, msg_no_lm
mov edi, 0xB8000 + 160  ; next line
call print_pm
jmp .hang
.hang:
hlt
jmp .hang
; ----------------------------
; VGA print routine (32-bit)
; ----------------------------
print_pm:                                   ;for deletion
.next_pm:                                   ;
mov al, [esi]
test al, al
jz .done_pm
mov [edi], al
mov byte [edi + 1], 0x07
add esi, 1
add edi, 2
jmp .next_pm
.done_pm:
ret                                         ;
; ----------------------------
; Long mode code (64-bit)
; ----------------------------
[bits 64]
long_mode_entry:
; Setup segments (flat)
mov ax, 0x10
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax
; Print message in long mode (VGA)
mov rsi, msg_lm
mov rdi, 0xB8000 + 320  ; a couple lines down
call print_lm
.hang64:
hlt
jmp .hang64
; ----------------------------
; VGA print routine (64-bit)
; ----------------------------
print_lm:                              ;for deletion
.next_lm:                              ;
mov al, [rsi]
test al, al
jz .done_lm
mov [rdi], al
mov byte [rdi + 1], 0x07
inc rsi
add rdi, 2
jmp .next_lm
.done_lm:
ret                                     ;
; ----------------------------
; Data & stack
; ----------------------------
[bits 16]
msg_real db "Entering protected mode...", 0
[bits 32]
msg_pm db "Hello from 32-bit protected mode!", 0
msg_no_lm db "Long mode not supported!", 0
[bits 64]
msg_lm db "Hello from 64-bit long mode!", 0
; ----------------------------
; 64-bit GDT setup
; ----------------------------
align 8
gdt64_start:
dq 0x0000000000000000               ; null
dq 0x00AF9A000000FFFF               ; 64-bit code (L=1, D=0)
dq 0x00CF92000000FFFF               ; data (compatible with 32-bit)
gdt64_end:
gdt64_descriptor:
dw gdt64_end - gdt64_start - 1
dd gdt64_start                      ; 32-bit address (upper bits zero in low memory)
; ----------------------------
; Page tables (aligned, zero-initialized except entries)
; ----------------------------
align 4096
pml4:
times 512 dq 0
pdpt:
times 512 dq 0
pd:
times 512 dq 0
align 16
stack_bottom:
times 4096 db 0        ; 4 KB stack
stack_top:
