#include "xgui.h"
#include "xeditor.h"
#include "xrun.h"
#include "xcalc.h"
#include "xstat.h"
#include "../kernel/gui.h"
#include "../kernel/io.h"
#include "../kernel/time.h"
#include "../kernel/power.h"
#include "../kernel/mouse.h"
#include "../xfs/xfs.h"

static void xgui_header() {
    gui_set_app_style('W', "Desktop");
    kprint_color(" Home  |  Apps  |  Settings\n", 0x0F);
    kprint_color(" --------------------------------------\n", 0x08);
    kprint_color(" Welcome back, Administrator\n\n", 0x03);
}

static void xgui_tiles() {
    kprint_color("[Pinned]\n", 0x0E);
    kprint("+---------------+   +---------------+\n");
    kprint("| [1] FILES [F] |   | [2] EDITOR [E]|\n");
    kprint("+---------------+   +---------------+\n");
    kprint("+---------------+   +---------------+\n");
    kprint("| [3] RUNNER[R] |   | [4] CALC   [C]|\n");
    kprint("+---------------+   +---------------+\n");
    kprint("+---------------+   +---------------+\n");
    kprint("| [5] MONITOR[S]|   | [6] CLOCK  [T]|\n");
    kprint("+---------------+   +---------------+\n");
    kprint("\n");
    kprint("      +-------------------------+\n");
    kprint("      | [7] POWER MENU      [P] |\n");
    kprint("      +-------------------------+\n");
}

static int in_rect(int x, int y, int x1, int y1, int x2, int y2) {
    return (x >= x1 && x <= x2 && y >= y1 && y <= y2);
}

static int tile_hit_test(int x, int y) {
    // Hitboxes aligned to actual text-drawn tile positions.
    if (in_rect(x, y, 8,   56, 143,  79)) return 1; // Files
    if (in_rect(x, y, 168, 56, 303,  79)) return 2; // Editor
    if (in_rect(x, y, 8,   80, 143, 103)) return 3; // Runner
    if (in_rect(x, y, 168, 80, 303, 103)) return 4; // Calc
    if (in_rect(x, y, 8,  104, 143, 127)) return 5; // Monitor
    if (in_rect(x, y, 168,104, 303, 127)) return 6; // Clock
    if (in_rect(x, y, 56, 136, 271, 151)) return 7; // Power
    return 0;
}

static void files_hub() {
    char cmd[16];
    char fname[32];
    char content[256];

    clear_screen();
    gui_set_app_style('F', "Files");
    kprint_color("Files Hub  ::  ls | mk | read | write | rm | back\n", 0x0A);

    while (1) {
        kprint("files> ");
        kinput(cmd);

        if (strcmp(cmd, "back") == 0) return;
        if (strcmp(cmd, "ls") == 0) {
            xfs_list();
        } else if (strcmp(cmd, "mk") == 0) {
            kprint("file: ");
            kinput(fname);
            if (xfs_create(fname) != -1) {
                xfs_sync();
                kprint_color("[ok] created\n", 0x0A);
            } else {
                kprint_color("[err] create failed\n", 0x0C);
            }
        } else if (strcmp(cmd, "read") == 0) {
            char* data;
            kprint("file: ");
            kinput(fname);
            data = xfs_read(fname);
            if (data) {
                kprint_color("content:\n", 0x0E);
                kprint(data);
                kprint("\n");
            } else {
                kprint_color("[err] not found\n", 0x0C);
            }
        } else if (strcmp(cmd, "write") == 0) {
            kprint("file: ");
            kinput(fname);
            kprint("text: ");
            kinput(content);
            if (xfs_write(fname, content) == 0) {
                xfs_sync();
                kprint_color("[ok] saved\n", 0x0A);
            } else {
                kprint_color("[err] write failed\n", 0x0C);
            }
        } else if (strcmp(cmd, "rm") == 0) {
            kprint("file: ");
            kinput(fname);
            xfs_remove(fname);
            xfs_sync();
            kprint_color("[ok] removed\n", 0x0A);
        } else {
            kprint_color("[err] unknown command\n", 0x0C);
        }
    }
}

static void launch_tile(int id) {
    char fname[32];

    if (id == 1) {
        files_hub();
    } else if (id == 2) {
        clear_screen();
        gui_set_app_style('E', "Editor");
        kprint("file: ");
        kinput(fname);
        xeditor_main(fname);
    } else if (id == 3) {
        clear_screen();
        gui_set_app_style('R', "Runner");
        kprint("script: ");
        kinput(fname);
        xrun_main(fname);
    } else if (id == 4) {
        xcalc_main();
    } else if (id == 5) {
        xstat_main();
    } else if (id == 6) {
        int h, m, s;
        clear_screen();
        gui_set_app_style('T', "Time Widget");
        get_time(&h, &m, &s);
        kprint_color("Current time: ", 0x0E);
        kprint_int(h); kprint(":");
        kprint_int(m); kprint(":");
        kprint_int(s); kprint("\n");
        kprint("Press ESC...");
        while (1) {
            if (mouse_poll()) {
                gui_draw_mouse(mouse_get_x(), mouse_get_y());
            }
            gui_draw_mouse(mouse_get_x(), mouse_get_y());
            { unsigned char st = inb(0x64); if ((st & 0x01) && !(st & 0x20) && inb(0x60) == 0x01) break; }
        }
    } else if (id == 7) {
        char cmd[16];
        clear_screen();
        gui_set_app_style('P', "Power");
        kprint("action (reboot/shutdown/back): ");
        kinput(cmd);
        if (strcmp(cmd, "reboot") == 0) sys_reboot();
        else if (strcmp(cmd, "shutdown") == 0) sys_shutdown();
    }
}

static void draw_desktop() {
    clear_screen();
    xgui_header();
    xgui_tiles();
    kprint_color("\nQuick tip: click a tile | ESC -> Terminal\n", 0x08);
}

void xgui_main() {
    int left_prev = 0;
    int pending_launch = 0;

    draw_desktop();
    gui_draw_mouse(mouse_get_x(), mouse_get_y());

    while (1) {
        // 1) Mouse phase
        if (mouse_poll()) {
            gui_draw_mouse(mouse_get_x(), mouse_get_y());
        }

        {
            int left_now = mouse_left_button();
            if (left_now && !left_prev) {
                int hit = tile_hit_test(mouse_get_x(), mouse_get_y());
                if (hit > 0) {
                    pending_launch = hit;
                }
            }
            left_prev = left_now;
        }

        // 2) Keyboard phase
        {
            unsigned char status = inb(0x64);
            if ((status & 0x01) && !(status & 0x20)) {
                unsigned char sc = inb(0x60);
                if (sc == 0x01) {
                    clear_screen();
                    gui_set_app_style('T', "Terminal");
                    kprint_color("XGUI closed\n", 0x0A);
                    return;
                }
            }
        }

        // 3) Program launch phase
        if (pending_launch > 0) {
            launch_tile(pending_launch);
            pending_launch = 0;
            draw_desktop();
            gui_draw_mouse(mouse_get_x(), mouse_get_y());
        }
    }
}
