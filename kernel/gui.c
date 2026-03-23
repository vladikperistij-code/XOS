#include "gui.h"
#include "time.h"
#include "mouse.h"

#define FB_ADDRESS 0xA0000
#define SCREEN_W 320
#define SCREEN_H 200
#define FONT_W 8
#define FONT_H 8
#define TOPBAR_H 12
#define TASKBAR_H 12
#define PADDING_X 8
#define CONTENT_Y (TOPBAR_H + 4)
#define CONTENT_H (SCREEN_H - TOPBAR_H - TASKBAR_H - 6)
#define MAX_COLS ((SCREEN_W - (PADDING_X * 2)) / FONT_W)
#define MAX_ROWS (CONTENT_H / FONT_H)

static unsigned char* fb = (unsigned char*)FB_ADDRESS;
static unsigned char* bios_font = (unsigned char*)0x000FFA6E;

static int cursor_x = 0;
static int cursor_y = 0;
int current_color = 0x0F;
static char app_title[24] = "Terminal";
static char app_icon = 'T';
static int mouse_prev_x = -1;
static int mouse_prev_y = -1;
static unsigned char mouse_backup[64];

static unsigned char vga16_to_256[16] = {
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x14, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F
};

static void draw_char_at_px(int px, int py, char c, unsigned char color);
static void draw_topbar_only(void);
static void draw_taskbar_clock(void);

static void draw_text_px(int x, int y, const char* s, unsigned char color) {
    int i = 0;
    while (s[i]) {
        draw_char_at_px(x + i * FONT_W, y, s[i], color);
        i++;
    }
}

static void draw_2digit_px(int x, int y, int v, unsigned char color) {
    char t[3];
    t[0] = '0' + ((v / 10) % 10);
    t[1] = '0' + (v % 10);
    t[2] = '\0';
    draw_text_px(x, y, t, color);
}

static void text_copy(char* dst, char* src, int max) {
    int i = 0;
    while (i < max - 1 && src[i]) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

static void memory_copy(unsigned char* source, unsigned char* dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) dest[i] = source[i];
}

static unsigned char fg_color() {
    unsigned char idx = (unsigned char)(current_color & 0x0F);
    return vga16_to_256[idx];
}

static void put_pixel(int x, int y, unsigned char color) {
    if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) return;
    fb[y * SCREEN_W + x] = color;
}

static void fill_rect(int x, int y, int w, int h, unsigned char color) {
    int iy, ix;
    for (iy = 0; iy < h; iy++) {
        for (ix = 0; ix < w; ix++) {
            put_pixel(x + ix, y + iy, color);
        }
    }
}

static void draw_char_at_px(int px, int py, char c, unsigned char color) {
    int row, col;
    unsigned char glyph;
    unsigned char* glyph_ptr;
    if ((unsigned char)c < 32 || (unsigned char)c > 126) c = '?';
    glyph_ptr = bios_font + ((unsigned char)c * FONT_H);

    for (row = 0; row < FONT_H; row++) {
        glyph = glyph_ptr[row];
        for (col = 0; col < FONT_W; col++) {
            if (glyph & (0x80 >> col)) {
                put_pixel(px + col, py + row, color);
            }
        }
    }
}

static void draw_ui_chrome() {
    int y;
    for (y = 0; y < SCREEN_H - TASKBAR_H; y++) {
        unsigned char c = (y < 40) ? 0x11 : (y < 80 ? 0x19 : (y < 120 ? 0x12 : 0x01));
        fill_rect(0, y, SCREEN_W, 1, c);
    }

    for (y = 0; y < SCREEN_H - TASKBAR_H; y += 3) {
        fill_rect(0, y, SCREEN_W, 1, 0x18);
    }

    fill_rect(0, SCREEN_H - TASKBAR_H, SCREEN_W, TASKBAR_H, 0x08);
    fill_rect(2, SCREEN_H - TASKBAR_H + 1, 42, TASKBAR_H - 2, 0x01);
    fill_rect(50, SCREEN_H - TASKBAR_H + 1, 88, TASKBAR_H - 2, 0x18);
    fill_rect(252, SCREEN_H - TASKBAR_H + 1, 66, TASKBAR_H - 2, 0x18);

    fill_rect(PADDING_X - 3, CONTENT_Y - 3, MAX_COLS * FONT_W + 6, CONTENT_H + 6, 0x08);
    fill_rect(PADDING_X - 2, CONTENT_Y - 2, MAX_COLS * FONT_W + 4, CONTENT_H + 4, 0x07);
    fill_rect(PADDING_X - 1, CONTENT_Y - 1, MAX_COLS * FONT_W + 2, CONTENT_H + 2, 0x01);

    draw_topbar_only();
    draw_text_px(8, SCREEN_H - TASKBAR_H + 2, "START", 0x0F);
    draw_text_px(56, SCREEN_H - TASKBAR_H + 2, "XOS DESKTOP", 0x0F);
    draw_taskbar_clock();
}

static void draw_topbar_only(void) {
    char icon_text[2];
    int bar_y = CONTENT_Y - TOPBAR_H - 1;
    fill_rect(PADDING_X - 1, bar_y, MAX_COLS * FONT_W + 2, TOPBAR_H, 0x09);
    fill_rect(PADDING_X, bar_y + 1, MAX_COLS * FONT_W, TOPBAR_H - 2, 0x01);
    draw_text_px(PADDING_X + 4, bar_y + 2, "XWindow", 0x0F);

    fill_rect(PADDING_X + 92, bar_y + 1, MAX_COLS * FONT_W - 92, TOPBAR_H - 2, 0x18);
    icon_text[0] = app_icon;
    icon_text[1] = '\0';
    draw_text_px(PADDING_X + 98, bar_y + 2, icon_text, 0x0E);
    draw_text_px(PADDING_X + 110, bar_y + 2, app_title, 0x0F);
}

static void draw_taskbar_clock(void) {
    int h, m, s;
    get_time(&h, &m, &s);
    draw_2digit_px(256, SCREEN_H - TASKBAR_H + 2, h, 0x0F);
    draw_text_px(272, SCREEN_H - TASKBAR_H + 2, ":", 0x0F);
    draw_2digit_px(280, SCREEN_H - TASKBAR_H + 2, m, 0x0F);
    draw_text_px(296, SCREEN_H - TASKBAR_H + 2, ":", 0x0F);
    draw_2digit_px(304, SCREEN_H - TASKBAR_H + 2, s, 0x0F);
}

void gui_set_app_style(char icon, char* title) {
    app_icon = icon;
    text_copy(app_title, title, sizeof(app_title));
    draw_topbar_only();
}

static void erase_text_cell(int cx, int cy) {
    int px = PADDING_X + (cx * FONT_W);
    int py = CONTENT_Y + (cy * FONT_H);
    fill_rect(px, py, FONT_W, FONT_H, 0x10);
}

static void draw_cell_char(int cx, int cy, char c) {
    int px = PADDING_X + (cx * FONT_W);
    int py = CONTENT_Y + (cy * FONT_H);
    erase_text_cell(cx, cy);
    draw_char_at_px(px, py, c, fg_color());
}

static void scroll() {
    int y;
    int src_y = CONTENT_Y + FONT_H;
    int dst_y = CONTENT_Y;
    int bytes_per_line = SCREEN_W;
    int lines_to_move = CONTENT_H - FONT_H;

    for (y = 0; y < lines_to_move; y++) {
        memory_copy(
            fb + (src_y + y) * bytes_per_line,
            fb + (dst_y + y) * bytes_per_line,
            bytes_per_line
        );
    }

    fill_rect(PADDING_X, CONTENT_Y + CONTENT_H - FONT_H, MAX_COLS * FONT_W, FONT_H, 0x10);
    cursor_y = MAX_ROWS - 1;
}

void clear_screen() {
    draw_ui_chrome();
    fill_rect(PADDING_X, CONTENT_Y, MAX_COLS * FONT_W, CONTENT_H, 0x10);
    cursor_x = 0;
    cursor_y = 0;
    mouse_prev_x = -1;
    mouse_prev_y = -1;
    gui_draw_mouse(mouse_get_x(), mouse_get_y());
}

void gui_draw_mouse(int x, int y) {
    int i, px, py;
    static unsigned char mask[8][8] = {
        {1,0,0,0,0,0,0,0},
        {1,1,0,0,0,0,0,0},
        {1,1,1,0,0,0,0,0},
        {1,1,1,1,0,0,0,0},
        {1,1,1,1,1,0,0,0},
        {1,1,1,1,1,1,0,0},
        {1,1,0,1,1,0,0,0},
        {1,0,0,0,1,0,0,0}
    };

    // Avoid redraw flicker when cursor did not move.
    if (mouse_prev_x == x && mouse_prev_y == y) return;

    if (mouse_prev_x >= 0) {
        for (i = 0; i < 64; i++) {
            px = mouse_prev_x + (i % 8);
            py = mouse_prev_y + (i / 8);
            if (px >= 0 && px < SCREEN_W && py >= 0 && py < SCREEN_H) {
                fb[py * SCREEN_W + px] = mouse_backup[i];
            }
        }
    }

    for (i = 0; i < 64; i++) {
        px = x + (i % 8);
        py = y + (i / 8);
        if (px >= 0 && px < SCREEN_W && py >= 0 && py < SCREEN_H) {
            mouse_backup[i] = fb[py * SCREEN_W + px];
        } else {
            mouse_backup[i] = 0;
        }
    }

    for (i = 0; i < 64; i++) {
        px = x + (i % 8);
        py = y + (i / 8);
        if (px >= 0 && px < SCREEN_W && py >= 0 && py < SCREEN_H && mask[i / 8][i % 8]) {
            fb[py * SCREEN_W + px] = 0x0F;
        }
    }

    mouse_prev_x = x;
    mouse_prev_y = y;
}

void print_char(char c) {
    if (current_color == 0) current_color = 0x0F;

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            erase_text_cell(cursor_x, cursor_y);
        }
    } else {
        draw_cell_char(cursor_x, cursor_y, c);
        cursor_x++;
    }

    if (cursor_x >= MAX_COLS) {
        cursor_x = 0;
        cursor_y++;
    }

    if (cursor_y >= MAX_ROWS) scroll();
    gui_draw_mouse(mouse_get_x(), mouse_get_y());
}

void kprint(char* message) {
    int i;
    for (i = 0; message[i] != '\0'; i++) print_char(message[i]);
}

void kprint_color(char* message, int color) {
    int old = current_color;
    current_color = (color == 0) ? 0x0F : color;
    kprint(message);
    current_color = old;
}

int strcmp(char* s1, char* s2) {
    int i = 0;
    while (s1[i] == s2[i]) {
        if (s1[i] == '\0') return 0;
        i++;
    }
    return s1[i] - s2[i];
}

int strlen(char* s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

void kprint_int(int n) {
    char s[12];
    int i = 0;
    if (n == 0) {
        print_char('0');
        return;
    }
    if (n < 0) {
        print_char('-');
        n = -n;
    }
    while (n > 0 && i < 11) {
        s[i++] = (n % 10) + '0';
        n /= 10;
    }
    while (--i >= 0) print_char(s[i]);
}
