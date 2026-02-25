#include "xcalc.h"
#include "../kernel/gui.h"
#include "../kernel/io.h"

// Допоміжна функція: текст у число
int x_atoi(char* s) {
    int res = 0;
    for (int i = 0; s[i] >= '0' && s[i] <= '9'; ++i)
        res = res * 10 + s[i] - '0';
    return res;
}

void xcalc_main() {
    char n1_buf[16], n2_buf[16], op_buf[4];
    int a, b, res;

    clear_screen();
    kprint_color("=== XOS Calculator v1.2 ===\n", 0x0B);
    
    kprint("Enter first number: ");
    kinput(n1_buf);
    a = x_atoi(n1_buf);

    kprint("Enter operation (+, -, *, /): ");
    kinput(op_buf);

    kprint("Enter second number: ");
    kinput(n2_buf);
    b = x_atoi(n2_buf);

    kprint("\nResult: ");
    if (op_buf[0] == '+')      res = a + b;
    else if (op_buf[0] == '-') res = a - b;
    else if (op_buf[0] == '*') res = a * b;
    else if (op_buf[0] == '/') {
        if (b != 0) res = a / b;
        else { kprint_color("Error: Div by 0", 0x0C); return; }
    } else {
        kprint("Unknown op."); return;
    }

    kprint_int(res);
    while(!(inb(0x64) & 0x01)); inb(0x60);
    
}
