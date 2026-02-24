#include "gui.h"

#define VIDEO_ADDRESS 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80

int cursor_x = 0;
int cursor_y = 0;
// ставимо початковий колір (білий на чорному)
int current_color = 0x0F; 

void memory_copy(char* source, char* dest, int nbytes) {
    for (int i = 0; i < nbytes; i++) {
        dest[i] = source[i];
    }
}

void scroll() {
    char* vidmem = (char*)VIDEO_ADDRESS;
    // Піднімаємо текст на один рядок вгору
    memory_copy(vidmem + MAX_COLS * 2, vidmem, MAX_COLS * (MAX_ROWS - 1) * 2);
    
    // Очищаємо останній рядок правильним кольором
    for (int i = 0; i < MAX_COLS * 2; i += 2) {
        int offset = (MAX_ROWS - 1) * MAX_COLS * 2 + i;
        vidmem[offset] = ' ';
        vidmem[offset + 1] = current_color;
    }
    cursor_y = MAX_ROWS - 1;
}

void clear_screen() {
    char* vidmem = (char*)VIDEO_ADDRESS;
    // Якщо current_color випадково став 0, виправляємо його на білий
    if (current_color == 0) current_color = 0x0F;

    for (int i = 0; i < MAX_ROWS * MAX_COLS * 2; i += 2) {
        vidmem[i] = ' ';
        vidmem[i+1] = current_color;
    }
    cursor_x = 0; 
    cursor_y = 0;
}

void print_char(char c) {
    char* vidmem = (char*)VIDEO_ADDRESS;
    if (current_color == 0) current_color = 0x0F;

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            int offset = (cursor_y * MAX_COLS + cursor_x) * 2;
            vidmem[offset] = ' ';
            vidmem[offset + 1] = current_color;
        }
    } else {
        int offset = (cursor_y * MAX_COLS + cursor_x) * 2;
        vidmem[offset] = c;
        vidmem[offset + 1] = current_color;
        cursor_x++;
    }

    if (cursor_x >= MAX_COLS) {
        cursor_x = 0;
        cursor_y++;
    }

    if (cursor_y >= MAX_ROWS) {
        scroll();
    }
}

void kprint(char* message) {
    for (int i = 0; message[i] != '\0'; i++) {
        print_char(message[i]);
    }
}

void kprint_color(char* message, int color) {
    int old = current_color;
    // Захист: не даємо встановити чорний колір на чорному фоні
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
    if (n == 0) {
        print_char('0');
        return;
    }
    char s[12]; // Буфер для числа
    int i = 0;
    while (n > 0) {
        s[i++] = (n % 10) + '0';
        n /= 10;
    }
    while (--i >= 0) {
        print_char(s[i]);
    }
}
