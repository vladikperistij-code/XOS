#include "xstat.h"
#include "../kernel/gui.h"
#include "../kernel/io.h"
#include "../kernel/mouse.h"
#include "../xfs/xfs.h"

void xstat_main() {
    unsigned int esp_val;
    int used;
    int f_count = 0;
    int i;
    unsigned char st;

    __asm__ volatile("mov %%esp, %0" : "=r"(esp_val));
    used = 0x90000 - (int)esp_val;

    for (i = 0; i < MAX_FILES; i++) {
        if (filesystem[i].used) f_count++;
    }

    st = inb(0x1F7);

    clear_screen();
    gui_set_app_style('S', "System Monitor");
    kprint_color("========================================\n", 0x0B);
    kprint_color("   XStat 2.0  |  System Dashboard\n", 0x0F);
    kprint_color("========================================\n\n", 0x0B);

    kprint_color("[memory]\n", 0x0E);
    kprint("stack base : 0x90000\n");
    kprint("esp        : "); kprint_int((int)esp_val); kprint("\n");
    kprint("stack used : "); kprint_int(used); kprint(" bytes\n\n");

    kprint_color("[storage]\n", 0x0E);
    kprint("files in xfs: "); kprint_int(f_count); kprint(" / 8\n\n");

    kprint_color("[hardware]\n", 0x0E);
    kprint("disk status : ");
    if (st == 0xFF) kprint_color("OFFLINE\n", 0x0C);
    else kprint_color("READY\n", 0x0A);

    kprint("\nPress ESC...");
    while (1) {
        if (mouse_poll()) {
            gui_draw_mouse(mouse_get_x(), mouse_get_y());
        }
        gui_draw_mouse(mouse_get_x(), mouse_get_y());
        { unsigned char st = inb(0x64); if ((st & 0x01) && !(st & 0x20) && inb(0x60) == 0x01) break; }
    }
}
