#include "io.h"
#include "gui.h"
#include "power.h"

void sys_reboot() {
    // Команда перезавантаження через контролер клавіатури
    outb(0x64, 0xFE); 
}

void sys_shutdown() {
    kprint("Shutting down...\n");
    __asm__ volatile("cli");

    // ACPI shutdown
    outw(0x604, 0x2000);  
    outw(0xB004, 0x2000); 

    kprint("System halted.\n");
    while(1) {
        __asm__ volatile("hlt");
    }
}
