; boot16.asm — BIOS boot sector (512 bytes)
[org 0x7C00]
[bits 16]
; --- basic state: stack + segments ---
cli
xor ax, ax
mov ss, ax
mov sp, 0x7C00          ; simple safe stack under us
mov ds, ax
mov es, ax
sti
mov [boot_drive], dl    ; BIOS passed boot drive in DL
; --- (optional) reset disk ---
mov ah, 0x00
mov dl, [boot_drive]
int 0x13
; --- read stage 2 using LBA into 0000:1000 ---
mov si, dap           ; DS:SI → dap
mov dl, [boot_drive]  ; BIOS drive
mov ah, 0x42          ; extended read
int 0x13
jmp 0x0000:0x1000       ; jump to loaded stage-2 code
; --- data decleration --- ;
dap:
    db 16, 0        ; sizeof packet, reserved
    dw 10           ; sectors (filled later)
    dw 0x0000       ; offset
    dw 0x1000       ; segment
    dq 1            ; LBA
boot_drive db 0
times 510 - ($ - $$) db 0
dw 0xAA55
