; boot/boot16.asm — BIOS boot sector (ADD MEMORY DETECTION)
[org 0x7C00]
[bits 16]

; --- basic state: stack + segments ---
cli
xor ax, ax
mov ss, ax
mov sp, 0x7C00
mov ds, ax
mov es, ax
sti

mov [boot_drive], dl

; --- print loading message ---
mov si, msg_loading
call print_string

; --- reset disk ---
mov ah, 0x00
mov dl, [boot_drive]
int 0x13
jc disk_error

; --- DETECT MEMORY USING INT 0x15, EAX=0xE801 ---
; This gets memory in 1KB and 64KB blocks
mov ax, 0xE801
int 0x15
jc .no_e801

; AX = KB between 1MB and 16MB
; BX = 64KB blocks above 16MB
; Store in memory for kernel to read later
mov [0x9000], ax      ; Store extended memory 1 (1KB blocks)
mov [0x9004], bx      ; Store extended memory 2 (64KB blocks)
jmp .mem_detected

.no_e801:
; Fallback: assume 32MB if detection fails
mov word [0x9000], 0x3C00  ; 15MB in KB blocks (15 * 1024)
mov word [0x9004], 0x0100  ; 16MB in 64KB blocks (256 blocks)

.mem_detected:

; --- read stage 2 using LBA into 0000:1000 ---
mov si, dap_stage2
mov dl, [boot_drive]
mov ah, 0x42
int 0x13
jc disk_error

; --- load kernel sectors to 0x10000 ---
mov si, dap_kernel
mov dl, [boot_drive]
mov ah, 0x42
int 0x13
jc disk_error

; --- success! ---
mov si, msg_success
call print_string
mov dl, [boot_drive]
jmp 0x0000:0x1000

; --- error handler ---
disk_error:
    mov si, msg_error
    call print_string
    cli
    hlt

; --- print null-terminated string ---
print_string:
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

; --- data ---
msg_loading: db "Loading OS...", 13, 10, 0
msg_success: db "OK", 13, 10, 0
msg_error:   db "Disk error!", 13, 10, 0

dap_stage2:
    db 0x10
    db 0
    dw 32
    dw 0x1000
    dw 0x0000
    dq 1

dap_kernel:
    db 0x10
    db 0
    dw 100
    dw 0x0000
    dw 0x1000
    dq 33

boot_drive: db 0

times 510 - ($ - $$) db 0
dw 0xAA55