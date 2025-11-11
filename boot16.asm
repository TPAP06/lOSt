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

    ; --- read stage 2 at CHS (0,0,2) into 0000:1000 ---
    mov bx, 0x1000          ; buffer offset
    mov ah, 0x02            ; INT 13h: read sectors
    mov al, 1               ; read 1 sectors
    mov ch, 0               ; cylinder 0
    mov cl, 2               ; sector 2 (sector numbers start at 1)
    mov dh, 0               ; head 0
    mov dl, [boot_drive]    ; same boot drive
    int 0x13
    jc  disk_error

    jmp 0x0000:0x1000       ; jump to loaded stage-2 code

disk_error:
    mov si, msg
    call print
    jmp $

print:
    ; DS is 0x0000 from above; SI points to string
    mov ah, 0x0E            ; teletype output
.next:
    lodsb                   ; AL = [DS:SI], SI++
    or  al, al
    jz  .done
    int 0x10
    jmp .next
.done:
    ret

msg db "Disk read error!", 0
boot_drive db 0                                 ;  0   <-- happy franky!!!
                                                ; \|/ 
times 510 - ($ - $$) db 0                       ;  | 
dw 0xAA55                                       ; / \