; stage2.asm — Loaded at 0x1000, enables A20, loads kernel, preps gdt, enters 32-bit then 64-bit mode
[org 0x1000]
[bits 16]

; --- Enable A20 for >1Mib RAM --- ;
in al, 0x92             ; "in"/"out" are instructions that let CPU communicate with I/O ports
or al, 2
out 0x92, al
; --- load kernel --- ;
mov si, dap           ; DS:SI → dap
mov ah, 0x42          ; extended read
; int 0x13              ; dl is alread [boot_drive] from stage 1

start:
cli
xor ax, ax
mov ds, ax
mov es, ax
; --- Enter protected mode --- ;
lgdt [gdt32_descriptor]     ; Load GDT
mov eax, cr0
or eax, 1                 ; set PE bit
mov cr0, eax
jmp 0x08:protected_mode_entry   ; far jump to code selector

[bits 32]
protected_mode_entry:
; --- Setup data segments & stack --- ;
mov ax, 0x10       ; data selector
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax
mov esp, stack_top ; initialize 32-bit stack
; Setup page tables for long mode (identity map first 2MB as example)
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
; Load 64-bit GDT and jump to long mode
lgdt [gdt64_descriptor]
jmp 0x08:long_mode_entry  ; far jump to 64-bit code selector

[bits 64]
long_mode_entry:
; --- Setup segments (flat) --- ;
mov ax, 0x10
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

mov rsp, stack_top  ; set up 64-bit stack pointer

; --- jump to kernel --- ;
; mov rax, 0x20000
; jmp rax

.hang64:
hlt
jmp .hang64

; --- Data & stack --- ;
[bits 16]
; --- sector loading through LBA --- ;
dap:
    db 16, 0        ; sizeof packet, reserved
    dw 40           ; sectors (filled later)
    dw 0x0000       ; offset
    dw 0x2000       ; segment
    dq 100          ; LBA

; --- GDT setup (32-bit) --- ;
align 4
gdt32_start:
dq 0x0000000000000000   ; null
dq 0x00CF9A000000FFFF   ; code: base=0, limit=4GB, 32-bit
dq 0x00CF92000000FFFF   ; data: base=0, limit=4GB, 32-bit
gdt32_end:
gdt32_descriptor:
dw gdt32_end - gdt32_start - 1
dd gdt32_start

[bits 64]
; --- GDT setup (64-bit)--- ;
align 8
gdt64_start:
dq 0x0000000000000000               ; null
dq 0x00AF9A000000FFFF               ; 64-bit code (L=1, D=0)
dq 0x00CF92000000FFFF               ; data (compatible with 32-bit)
gdt64_end:
gdt64_descriptor:
dw gdt64_end - gdt64_start - 1
dd gdt64_start                      ; 32-bit address (upper bits zero in low memory)
; --- Page tables (aligned, zero-initialized except entries) --- ;
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
