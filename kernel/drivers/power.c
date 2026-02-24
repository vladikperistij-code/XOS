#include "io.h"
#include "gui.h"
#include "power.h"

void sys_reboot() {
    kprint("Rebooting...\n");
    while (inb(0x64) & 0x02);
    outb(0x64, 0xFE);
}

void sys_shutdown() {
    kprint("Shutting down...\n");
    outb(0x604, 0x2000);
    __asm__ volatile("cli; hlt");
}
