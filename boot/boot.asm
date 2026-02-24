[bits 16]
[org 0x7c00]
KERNEL_OFFSET equ 0x1000

start:
    mov [BOOT_DRIVE], dl
    
    ; Вивід "XOS"
    mov ah, 0x0e
    mov al, 'X'
    int 0x10
    mov al, 'O'
    int 0x10
    mov al, 'S'
    int 0x10

    mov bx, KERNEL_OFFSET
    mov dh, 15
    mov dl, [BOOT_DRIVE]
    call disk_load

    ; ДОДАЙ ЦЕ ДЛЯ ТЕСТУ:
    mov ah, 0x0e
    mov al, 'D'
    int 0x10

    call switch_to_pm

    ; Завантаження ядра
    mov bx, KERNEL_OFFSET
    mov dh, 15
    mov dl, [BOOT_DRIVE]
    call disk_load

    ; Перехід
    call switch_to_pm
    jmp $

%include "boot/gdt.asm"
%include "boot/disk_read.asm"
%include "boot/switch_to_pm.asm"

[bits 32]
BEGIN_PM:
    call KERNEL_OFFSET
    jmp $

BOOT_DRIVE db 0
times 510-($-$$) db 0
dw 0xaa55
