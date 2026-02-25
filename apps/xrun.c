#include "xrun.h"
#include "../kernel/gui.h"
#include "../kernel/io.h"
#include "../xfs/xfs.h"
#include "../kernel/time.h"
#include "../kernel/power.h"
#include "xcalc.h"
#include "xstat.h"

// Функція затримки для команд beep та sleep
void xrun_delay(int count) {
    volatile int i;
    for (i = 0; i < count * 2000000; i++) {
        __asm__("nop");
    }
}

void xrun_main(char* filename) {
    char* script = xfs_read(filename);
    if (!script) {
        kprint("XRun Error: Script not found.\n");
        return;
    }

    char line[64];
    int s_ptr = 0, l_ptr = 0;

    kprint_color("[XRun Starting Execution...]\n", 0x0D);

    while (1) {
        char c = script[s_ptr++];

        if (c == '\n' || c == '\0' || c == '\r') {
            if (l_ptr > 0) {
                line[l_ptr] = '\0';

                // --- ПОВНИЙ ПЕРЕЛІК КОМАНД ---
                
                // 1. Системні
                if (strcmp(line, "clear") == 0) clear_screen();
                else if (strcmp(line, "ver") == 0) kprint("XOS v1.3 Auto-Shell\n");
                else if (strcmp(line, "reboot") == 0) sys_reboot();
                else if (strcmp(line, "shutdown") == 0) sys_shutdown();
                
                // 2. Час та Статус
                else if (strcmp(line, "time") == 0) {
                    int h, m, s; get_time(&h, &m, &s);
                    kprint("Script Time: "); kprint_int(h); kprint(":"); kprint_int(m); kprint("\n");
                }
                else if (strcmp(line, "stat") == 0) xstat_main();
                
                // 3. Звук (Ексклюзив XRun)
                else if (strcmp(line, "beep") == 0) {
                    play_sound(600); xrun_delay(1); nosound();
                }
                else if (strcmp(line, "sleep") == 0) {
                    xrun_delay(2);
                }

                // 4. Математика та Файли
                else if (strcmp(line, "calc") == 0) xcalc_main();
                else if (strcmp(line, "ls") == 0) xfs_list();
                
                // 5. Повідомлення
                else if (strcmp(line, "hello") == 0) {
                    kprint_color("XOS: Executing automated task...\n", 0x0A);
                }
                
                else {
                    kprint("XRun Unknown: "); kprint(line); kprint("\n");
                }
            }
            l_ptr = 0;
            if (c == '\0') break;
        } 
        else if (l_ptr < 63) {
            line[l_ptr++] = c;
        }
    }
    kprint_color("[XRun Execution Finished]\n", 0x0D);
}
