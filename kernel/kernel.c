#include "gui.h"
#include "time.h"
#include "mouse.h"
#include "../xfs/xfs.h"
#include "power.h"
#include "../apps/xstat.h"
#include "../apps/xgui.h"

void main() {
    char buf[128];
    int h, m, s;

    xfs_init();
    xfs_load();
    mouse_init();

    clear_screen();
    gui_set_app_style('T', "Terminal");
    kprint_color("XOS Kernel v1.4 [XTechnologies]\n", 0x0E);
    kprint_color("Launching XGUI...\n", 0x0A);
    xgui_main();
    clear_screen();
    gui_set_app_style('T', "Terminal");
    kprint_color("XGUI closed. Terminal ready.\n", 0x0E);

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
                xfs_sync();
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
        else if (strcmp(buf, "edit") == 0) {
            kprint("File to edit: ");
            char fname[32];
            kinput(fname);
            xeditor_main(fname);
        }
        else if (strcmp(buf, "run") == 0) {
            kprint("Script name: ");
            char fname[32];
            kinput(fname);
            xrun_main(fname);
        }
        else if (strcmp(buf, "stat") == 0) {
            xstat_main();
        }
        else if (strcmp(buf, "calc") == 0) {
            xcalc_main();
        }
        else if (strcmp(buf, "xgui") == 0) {
            xgui_main();
        }
        else if (strcmp(buf, "help") == 0) {
            kprint("Available commands:\n\n");

            kprint("- ls: List all files"); kprint("         -  write: Write content to file\n");
            kprint("- mkfile: Create a new file"); kprint("  -  ver: Show kernel version\n");
            kprint("- rm: Remove a file"); kprint("          -  edit: Edit a file with XEditor\n");
            kprint("- time: Show current time"); kprint("    -  run: Run a script file with XRun\n");
            kprint("- clear: Clear screen"); kprint("        -  stat: Show system status with XStat\n");
            kprint("- shutdown: Shutdown system"); kprint("  -  calc: open calculator\n");
            kprint("- reboot: Reboot system"); kprint("      -  read: Read file content\n");
            kprint("- xgui: Open desktop shell GUI\n");
        }
        else {
            kprint("Unknown command.\n");
        }
    }
}
