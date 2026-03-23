#include "xcalc.h"
#include "../kernel/gui.h"
#include "../kernel/io.h"
#include "../kernel/mouse.h"

int x_atoi(char* s) {
    int sign = 1;
    int res = 0;
    int i = 0;
    if (s[0] == '-') {
        sign = -1;
        i = 1;
    }
    for (; s[i] >= '0' && s[i] <= '9'; ++i) {
        res = res * 10 + s[i] - '0';
    }
    return res * sign;
}

void xcalc_main() {
    char n1_buf[16], n2_buf[16], op_buf[4];
    int a, b, res;

    clear_screen();
    gui_set_app_style('C', "Calculator");
    kprint_color("========================================\n", 0x0B);
    kprint_color("   XCalc 2.0  |  Glass Calculator\n", 0x0F);
    kprint_color("========================================\n\n", 0x0B);

    kprint("first number: ");
    kinput(n1_buf);
    a = x_atoi(n1_buf);

    kprint("operation (+ - * /): ");
    kinput(op_buf);

    kprint("second number: ");
    kinput(n2_buf);
    b = x_atoi(n2_buf);

    kprint_color("\nresult: ", 0x0E);
    if (op_buf[0] == '+') res = a + b;
    else if (op_buf[0] == '-') res = a - b;
    else if (op_buf[0] == '*') res = a * b;
    else if (op_buf[0] == '/') {
        if (b != 0) res = a / b;
        else {
            kprint_color("error: division by zero\n", 0x0C);
            return;
        }
    } else {
        kprint_color("error: unknown op\n", 0x0C);
        return;
    }

    kprint_int(res);
    kprint("\n\nPress ESC...");
    while (1) {
        if (mouse_poll()) {
            gui_draw_mouse(mouse_get_x(), mouse_get_y());
        }
        gui_draw_mouse(mouse_get_x(), mouse_get_y());
        { unsigned char st = inb(0x64); if ((st & 0x01) && !(st & 0x20) && inb(0x60) == 0x01) break; }
    }
}
