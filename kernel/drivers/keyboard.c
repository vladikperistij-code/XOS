#include "gui.h"
#include "io.h"
#include "mouse.h"

// Глобальний прапорець стану Shift
static int shift_active = 0;

char get_char_from_scancode(unsigned char scancode) {
    // 1. Обробка натискання Shift (LShift: 0x2A, RShift: 0x36)
    if (scancode == 0x2A || scancode == 0x36) {
        shift_active = 1;
        return 0;
    }
    // 2. Обробка відпускання Shift (LShift release: 0xAA, RShift release: 0xB6)
    if (scancode == 0xAA || scancode == 0xB6) {
        shift_active = 0;
        return 0;
    }

    // Якщо це відпускання будь-якої іншої клавіші (біт 7 встановлено) - ігноруємо
    if (scancode & 0x80) return 0;

    // --- ЛОГІКА З ЗАЖАТИМ SHIFT ---
    if (shift_active) {
        switch(scancode) {
            case 0x1A: return '{'; case 0x1B: return '}';
            // Цифри перетворюються на символи
            case 0x02: return '!'; case 0x03: return '@'; case 0x04: return '#';
            case 0x05: return '$'; case 0x06: return '%'; case 0x07: return '^';
            case 0x08: return '&'; case 0x09: return '*'; case 0x0A: return '(';
            case 0x0B: return ')'; case 0x0C: return '_'; case 0x0D: return '+';
            
            // Розділові знаки
            case 0x27: return ':'; case 0x28: return '"'; case 0x33: return '<';
            case 0x34: return '>'; case 0x35: return '?'; case 0x2B: return '|';

            // ВЕЛИКІ ЛІТЕРИ
            case 0x1E: return 'A'; case 0x30: return 'B'; case 0x2E: return 'C';
            case 0x20: return 'D'; case 0x12: return 'E'; case 0x21: return 'F';
            case 0x22: return 'G'; case 0x23: return 'H'; case 0x17: return 'I';
            case 0x24: return 'J'; case 0x25: return 'K'; case 0x26: return 'L';
            case 0x32: return 'M'; case 0x31: return 'N'; case 0x18: return 'O';
            case 0x19: return 'P'; case 0x10: return 'Q'; case 0x13: return 'R';
            case 0x1F: return 'S'; case 0x14: return 'T'; case 0x16: return 'U';
            case 0x2F: return 'V'; case 0x11: return 'W'; case 0x2D: return 'X';
            case 0x15: return 'Y'; case 0x2C: return 'Z';
            
            case 0x39: return ' '; // Пробіл залишається пробілом
            default: return 0;
        }
    }

    // --- ЗВИЧАЙНА РОЗКЛАДКА (SHIFT ВІДПУЩЕНО) ---
    switch(scancode) {
        case 0x1A: return '['; case 0x1B: return ']';
        case 0x02: return '1'; case 0x03: return '2'; case 0x04: return '3';
        case 0x05: return '4'; case 0x06: return '5'; case 0x07: return '6';
        case 0x08: return '7'; case 0x09: return '8'; case 0x0A: return '9';
        case 0x0B: return '0'; case 0x0C: return '-'; case 0x0D: return '=';

        case 0x1E: return 'a'; case 0x30: return 'b'; case 0x2E: return 'c';
        case 0x20: return 'd'; case 0x12: return 'e'; case 0x21: return 'f';
        case 0x22: return 'g'; case 0x23: return 'h'; case 0x17: return 'i';
        case 0x24: return 'j'; case 0x25: return 'k'; case 0x26: return 'l';
        case 0x32: return 'm'; case 0x31: return 'n'; case 0x18: return 'o';
        case 0x19: return 'p'; case 0x10: return 'q'; case 0x13: return 'r';
        case 0x1F: return 's'; case 0x14: return 't'; case 0x16: return 'u';
        case 0x2F: return 'v'; case 0x11: return 'w'; case 0x2D: return 'x';
        case 0x15: return 'y'; case 0x2C: return 'z';

        case 0x39: return ' ';  // Space
        case 0x1C: return '\n'; // Enter
        case 0x0E: return '\b'; // Backspace
        case 0x01: return 27;   // ESC

        case 0x27: return ';'; case 0x28: return '\''; case 0x33: return ',';
        case 0x34: return '.'; case 0x35: return '/'; case 0x2B: return '\\';
        
        default: return 0;
    }
}

void kinput(char* buffer) {
    int i = 0;
    while (1) {
        if (mouse_poll()) {
            gui_draw_mouse(mouse_get_x(), mouse_get_y());
        }
        gui_draw_mouse(mouse_get_x(), mouse_get_y());

        unsigned char status = inb(0x64);
        if (!(status & 0x01)) continue;
        if (status & 0x20) { inb(0x60); continue; } // drain mouse byte from shared 0x60 port
        unsigned char scancode = inb(0x60);
        
        char c = get_char_from_scancode(scancode);

        // Якщо це просто натискання Shift або відпускання будь-якої клавіші - не додаємо в буфер
        if (c == 0) continue;
        if (scancode & 0x80) continue; 

        if (c == '\n') {
            buffer[i] = '\0';
            print_char('\n');
            return;
        } else if (c == '\b') {
            if (i > 0) {
                i--;
                print_char('\b');
            }
        } else if (i < 120) {
            buffer[i++] = c;
            print_char(c);
        }
    }
}
