#include "xstat.h"
#include "../kernel/gui.h"
#include "../kernel/io.h"
#include "../xfs/xfs.h"

void xstat_main() {
    unsigned int esp_val;
    // Отримуємо значення ESP максимально безпечно
    __asm__ volatile("mov %%esp, %0" : "=r"(esp_val));

    clear_screen();
    kprint_color("=== XOS v1.2 SYSTEM MONITOR ===\n", 0x1F);
    
    kprint("\n[ MEMORY ]\n");
    kprint("  Stack Base  : 0x90000\n");
    kprint("  Current ESP : "); kprint_int((int)esp_val);
    
    // Розрахунок використання стеку
    int used = 0x90000 - (int)esp_val;
    kprint("\n  Stack Used  : "); kprint_int(used); kprint(" bytes\n");

    kprint("\n[ STORAGE ]\n");
    int f_count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem[i].used) f_count++;
    }
    kprint("  Files in XFS: "); kprint_int(f_count); kprint(" / 8\n");

    kprint("\n[ HARDWARE ]\n");
    unsigned char st = inb(0x1F7);
    kprint("  Disk Status : ");
    if (st == 0xFF) kprint_color("OFFLINE\n", 0x0C);
    else kprint_color("READY\n", 0x0A);

    kprint("\nPress ESC to exit...");
    while (1) {
        if (inb(0x64) & 0x01) {
            if (inb(0x60) == 0x01) break; // Вихід по ESC
        }
    }
    clear_screen();
}
