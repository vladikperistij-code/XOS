#include "gui.h"
#include "time.h"
#include "../xfs/xfs.h"
#include "power.h"

void main() {
    char buf[128];
    int h, m, s;

    xfs_init();
    xfs_load(); // Тепер це безпечно!

    clear_screen();
    kprint_color("XOS Kernel v0.3.5 [XTechnologies]\n", 0x0E);

    while(1) {
        kprint_color("admin@xos >> ", 0x0C);
        kinput(buf);

        if (strcmp(buf, "ls") == 0) {
            xfs_list();
        } 
        else if (strcmp(buf, "mkfile") == 0) {
            kprint("Filename: ");
            char fname[32];
            kinput(fname);
            if (xfs_create(fname) != -1) {
                xfs_sync(); // Спробуємо записати на диск
                kprint("File created.\n");
            }
        }
        else if (strcmp(buf, "time") == 0) {
            get_time(&h, &m, &s);
            kprint_int(h); kprint(":"); kprint_int(m); kprint("\n");
        }
        else if (strcmp(buf, "clear") == 0) {
            clear_screen();
        }
        else if (strcmp(buf, "ver") == 0) {
            kprint_color("XOS Kernel v1.0 - stable [XTechnologies]\n", 0x0E);
        }
        else if (strcmp(buf, "rm") == 0) {
            kprint("Filename: ");
            char fname[32];
            kinput(fname);
            xfs_remove(fname);
        }
        else if (strcmp(buf, "reboot") == 0) {
            sys_reboot();
        }
        else if (strcmp(buf, "shutdown") == 0) {
            sys_shutdown();
        }
        else if (strcmp(buf, "read") == 0) {
            kprint("Filename: ");
            char fname[32];
            kinput(fname);
            char* content = xfs_read(fname);
            if (content) {
                kprint("Content:\n");
                kprint(content);
                kprint("\n");
            } else {
                kprint("File not found.\n");
            }
        }
         else if (strcmp(buf, "write") == 0) {
            kprint("Filename: ");
            char fname[32];
            kinput(fname);
            kprint("Content: ");
            char content[256];
            kinput(content);
            if (xfs_write(fname, content) == 0) {
                xfs_sync();
                kprint("File written.\n");
            } else {
                kprint("File not found.\n");
            }
        }
        else if (strcmp(buf, "help") == 0) {
            kprint("Available commands:\n");
            kprint("- ls: List all files\n");
            kprint("- mkfile: Create a new file\n");
            kprint("- rm: Remove a file\n");
            kprint("- time: Show current time\n");
            kprint("- clear: Clear screen\n");
            kprint("- shutdown: Shutdown system\n");
            kprint("- reboot: Reboot system\n");
            kprint("- read: Read file content\n");
            kprint("- write: Write content to file\n");
            kprint("- ver: Show kernel version\n");
        }
        else {
            kprint("Unknown command");
        }
    }
}
