#include "xeditor.h"
#include "../kernel/gui.h"
#include "../kernel/io.h"
#include "../xfs/xfs.h"

void xeditor_main(char* filename) {
    char edit_buf[MAX_FILE_SIZE];
    int pos = 0;
    unsigned char scancode;

    clear_screen();
    kprint_color("[ XOS XEditor 1.1 ]", 0x4F); // Червоний фон для стилю
    kprint_color(" File: ", 0x07); kprint(filename);
    kprint_color(" | Mode: Scripting\n", 0x0E);
    kprint("--------------------------------------------------------------------------------\n");

    for(int i = 0; i < MAX_FILE_SIZE; i++) edit_buf[i] = 0;

    char* content = xfs_read(filename);
    if (content) {
        while (content[pos] != '\0' && pos < MAX_FILE_SIZE - 1) {
            edit_buf[pos] = content[pos];
            print_char(edit_buf[pos]);
            pos++;
        }
    }

    while (1) {
        if (inb(0x64) & 0x01) {
            scancode = inb(0x60);
            char c = get_char_from_scancode(scancode);
            if (scancode & 0x80) continue; 

            if (scancode == 0x01) break; // ESC
            
            if (scancode == 0x0E) { // BS
                if (pos > 0) { pos--; edit_buf[pos] = '\0'; print_char('\b'); }
            } else if (scancode == 0x1C) { // ENTER
                if (pos < MAX_FILE_SIZE - 1) { edit_buf[pos++] = '\n'; print_char('\n'); }
            } else if (c > 0 && pos < MAX_FILE_SIZE - 1) {
                edit_buf[pos++] = c;
                print_char(c);
            }
        }
    }

    edit_buf[pos] = '\0';
    xfs_create(filename);
    xfs_write(filename, edit_buf);
    xfs_sync();

    clear_screen();
    kprint_color("XEditor: Saved. Ready to run.\n", 0x0A);
}
