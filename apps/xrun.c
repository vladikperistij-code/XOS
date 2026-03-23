#include "xrun.h"
#include "../kernel/gui.h"
#include "../kernel/io.h"
#include "../xfs/xfs.h"
#include "../kernel/time.h"
#include "../kernel/power.h"
#include "xcalc.h"
#include "xstat.h"

void xrun_delay(int count) {
    volatile int i;
    for (i = 0; i < count * 2000000; i++) {
        __asm__("nop");
    }
}

void xrun_main(char* filename) {
    char* script = xfs_read(filename);
    char line[64];
    int s_ptr = 0, l_ptr = 0;

    clear_screen();
    gui_set_app_style('R', "Script Runner");
    kprint_color("========================================\n", 0x0B);
    kprint_color("   XRun 2.0  |  Script Engine\n", 0x0F);
    kprint_color("========================================\n", 0x0B);

    if (!script) {
        kprint_color("script not found\n", 0x0C);
        return;
    }

    kprint_color("executing...\n\n", 0x0E);

    while (1) {
        char c = script[s_ptr++];

        if (c == '\n' || c == '\0' || c == '\r') {
            if (l_ptr > 0) {
                line[l_ptr] = '\0';

                if (strcmp(line, "clear") == 0) clear_screen();
                else if (strcmp(line, "ver") == 0) kprint("XOS v2 script shell\n");
                else if (strcmp(line, "reboot") == 0) sys_reboot();
                else if (strcmp(line, "shutdown") == 0) sys_shutdown();
                else if (strcmp(line, "time") == 0) {
                    int h, m, s;
                    get_time(&h, &m, &s);
                    kprint("time: "); kprint_int(h); kprint(":"); kprint_int(m); kprint("\n");
                }
                else if (strcmp(line, "stat") == 0) xstat_main();
                else if (strcmp(line, "beep") == 0) {
                    play_sound(600); xrun_delay(1); nosound();
                }
                else if (strcmp(line, "sleep") == 0) xrun_delay(1000);
                else if (strcmp(line, "calc") == 0) xcalc_main();
                else if (strcmp(line, "ls") == 0) xfs_list();
                else if (strcmp(line, "hello") == 0) kprint_color("XOS: task done\n", 0x0A);
                else {
                    kprint("unknown: "); kprint(line); kprint("\n");
                }
            }

            l_ptr = 0;
            if (c == '\0') break;
        } else if (l_ptr < 63) {
            line[l_ptr++] = c;
        }
    }

    kprint_color("\nexecution finished\n", 0x0A);
}
