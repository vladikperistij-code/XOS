#include "gui.h"
#include "power.h"

void main() {
    int h, m, s;
    // Очищення буфера клавіатури при старті
    while (inb(0x64) & 0x01) { inb(0x60); }
    
    char buf[128]; 
    clear_screen();

    // Фірмове привітання XOS
    kprint_color("XOS Kernel v0.3 [XTechnologies]\n", 0x0E); // Жовтий
    kprint_color("System Ready. All drivers loaded.\n\n", 0x0A); // Зелений

    while(1) {
        kprint_color("admin@xos >>", 0x0C); // Червоний промпт
        kinput(buf);
        if (strcmp(buf, "help") == 0){
            kprint("-----------------------------\n");
            kprint("|reboot----------------------|\n");
            kprint("|shootdown-------------------|\n");
            kprint("|clear-----------------------|\n");
            kprint("|ver-------------------------|\n");
            kprint("-----------------------------\n");
        }
        else if (strcmp(buf, "reboot") == 0) {
            kprint("System rebooting...\n");
            sys_reboot();
        } 
        else if (strcmp(buf, "shutdown") == 0) {
            kprint("System shutting down...\n");
            sys_shutdown();
        }
        else if (strcmp(buf, "clear") == 0) {
            clear_screen();
        }
        else if (strcmp(buf, "ver") == 0) {
            kprint("XOS v0.3-Stable by XTechnologies\n");
        }
        
        else if (strlen(buf) > 0) {
            kprint("Unknown command: ");
            kprint(buf);
            kprint("\n");
        }
    }
}
