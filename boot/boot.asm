[bits 16]
[org 0x7c00]
KERNEL_OFFSET equ 0x1000

start:
    mov [BOOT_DRIVE], dl

    mov bp, 0x9000
    mov sp, bp

    mov ah, 0x0e
    mov al, 'X'
    int 0x10
    mov al, 'O'
    int 0x10
    mov al, 'S'
    int 0x10

    ; Switch to VGA 320x200x256 before protected mode
    mov ax, 0x0013
    int 0x10

    mov bx, KERNEL_OFFSET
    mov dh, 50
    mov dl, [BOOT_DRIVE]
    call disk_load

    call switch_to_pm
    jmp $

%include "boot/gdt.asm"
%include "boot/disk_read.asm"
%include "boot/switch_to_pm.asm"

[bits 32]
BEGIN_PM:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    call KERNEL_OFFSET
    jmp $

BOOT_DRIVE db 0
times 510-($-$$) db 0
dw 0xaa55
