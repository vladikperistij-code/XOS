#include "xeditor.h"
#include "../kernel/gui.h"
#include "../kernel/io.h"
#include "../kernel/mouse.h"
#include "../xfs/xfs.h"

void xeditor_main(char* filename) {
    char edit_buf[MAX_FILE_SIZE];
    int pos = 0;
    unsigned char scancode;

    clear_screen();
    gui_set_app_style('E', "Editor");
    kprint_color("========================================\n", 0x0B);
    kprint_color("   XEditor 2.0  |  Neon Write\n", 0x0F);
    kprint_color("========================================\n", 0x0B);
    kprint("file: "); kprint(filename);
    kprint("\nESC=save&exit\n\n");

    for (int i = 0; i < MAX_FILE_SIZE; i++) edit_buf[i] = 0;

    {
        char* content = xfs_read(filename);
        if (content) {
            while (content[pos] != '\0' && pos < MAX_FILE_SIZE - 1) {
                edit_buf[pos] = content[pos];
                print_char(edit_buf[pos]);
                pos++;
            }
        }
    }

    while (1) {
        if (mouse_poll()) {
            gui_draw_mouse(mouse_get_x(), mouse_get_y());
        }
        gui_draw_mouse(mouse_get_x(), mouse_get_y());

        {
            unsigned char status = inb(0x64);
            if (!(status & 0x01)) continue;
            if (status & 0x20) { inb(0x60); continue; }
            scancode = inb(0x60);
            {
                char c = get_char_from_scancode(scancode);
                if (scancode & 0x80) continue;

                if (scancode == 0x01) break;

                if (scancode == 0x0E) {
                    if (pos > 0) {
                        pos--;
                        edit_buf[pos] = '\0';
                        print_char('\b');
                    }
                } else if (scancode == 0x1C) {
                    if (pos < MAX_FILE_SIZE - 1) {
                        edit_buf[pos++] = '\n';
                        print_char('\n');
                    }
                } else if (c > 0 && pos < MAX_FILE_SIZE - 1) {
                    edit_buf[pos++] = c;
                    print_char(c);
                }
            }
        }
    }

    edit_buf[pos] = '\0';
    xfs_create(filename);
    xfs_write(filename, edit_buf);
    xfs_sync();

    clear_screen();
    kprint_color("XEditor: saved\n", 0x0A);
}
