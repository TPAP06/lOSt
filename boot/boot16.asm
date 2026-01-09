; boot/boot16.asm — BIOS boot sector (512 bytes)
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

; --- read stage 2 to 0x1000 ---
mov si, dap_stage2
mov dl, [boot_drive]
mov ah, 0x42
int 0x13
jc disk_error

; --- read kernel to 0x10000 (we'll move it later in protected mode) ---
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

disk_error:
    mov si, msg_error
    call print_string
    cli
    hlt

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

msg_loading: db "Loading OS...", 13, 10, 0
msg_success: db "OK", 13, 10, 0
msg_error:   db "Disk error!", 13, 10, 0

dap_stage2:
    db 0x10
    db 0
    dw 32               ; stage 2 size
    dw 0x1000           ; load to 0x1000
    dw 0x0000
    dq 1                ; sector 1

dap_kernel:
    db 0x10
    db 0
    dw 100              ; kernel size (50KB)
    dw 0x0000           ; load to 0x10000
    dw 0x1000
    dq 33               ; sector 33

boot_drive: db 0

times 510 - ($ - $$) db 0
dw 0xAA55