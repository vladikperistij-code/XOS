[bits 16]
[org 0x7c00]
KERNEL_OFFSET equ 0x1000

start:
    mov [BOOT_DRIVE], dl
    
    ; Налаштування стеку для 16-бітного режиму
    mov bp, 0x9000
    mov sp, bp

    ; Вивід "XOS" - знак того, що Bootloader живий
    mov ah, 0x0e
    mov al, 'X'
    int 0x10
    mov al, 'O'
    int 0x10
    mov al, 'S'
    int 0x10

    ; ЗАВАНТАЖЕННЯ ЯДРА (Збільшуємо кількість секторів!)
    mov bx, KERNEL_OFFSET
    mov dh, 50          ; Читаємо 50 секторів (25 КБ). Цього вистачить для XFS та ATA
    mov dl, [BOOT_DRIVE]
    call disk_load

    ; ПЕРЕХІД У PROTECTED MODE
    call switch_to_pm
    jmp $

%include "boot/gdt.asm"
%include "boot/disk_read.asm"
%include "boot/switch_to_pm.asm"

[bits 32]
BEGIN_PM:
    ; НАЛАШТУВАННЯ СЕГМЕНТІВ ТА СТЕКУ (Це лікує чорний екран!)
    mov ax, 0x10        ; Data segment зміщення в GDT
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000    ; Піднімаємо стек високо в пам'ять
    mov esp, ebp

    ; ВИКЛИК ЯДРА
    call KERNEL_OFFSET
    jmp $

BOOT_DRIVE db 0
times 510-($-$$) db 0
dw 0xaa55
