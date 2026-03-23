#include "io.h"
#include "mouse.h"

static int mouse_x = 160;
static int mouse_y = 100;
static int mouse_left = 0;
static int mouse_right = 0;

static unsigned char packet[3];
static int packet_index = 0;

static void mouse_wait_write() {
    int timeout = 100000;
    while (timeout-- > 0) {
        if ((inb(0x64) & 0x02) == 0) return;
    }
}

static void mouse_write(unsigned char value) {
    mouse_wait_write();
    outb(0x64, 0xD4);
    mouse_wait_write();
    outb(0x60, value);
}

static unsigned char mouse_read() {
    int timeout = 100000;
    while (timeout-- > 0) {
        if (inb(0x64) & 0x01) return inb(0x60);
    }
    return 0;
}

void mouse_init() {
    unsigned char status;

    mouse_wait_write();
    outb(0x64, 0xA8);

    mouse_wait_write();
    outb(0x64, 0x20);
    status = mouse_read();
    status |= 0x02;

    mouse_wait_write();
    outb(0x64, 0x60);
    mouse_wait_write();
    outb(0x60, status);

    mouse_write(0xF6);
    mouse_read();
    mouse_write(0xF4);
    mouse_read();
}

int mouse_poll() {
    unsigned char status;
    unsigned char data;
    int dx, dy;

    status = inb(0x64);
    if ((status & 0x01) == 0) return 0;
    if ((status & 0x20) == 0) return 0;

    data = inb(0x60);

    if (packet_index == 0 && (data & 0x08) == 0) return 0;
    packet[packet_index++] = data;
    if (packet_index < 3) return 0;
    packet_index = 0;

    dx = (signed char)packet[1];
    dy = (signed char)packet[2];

    mouse_x += dx;
    mouse_y -= dy;

    if (mouse_x < 0) mouse_x = 0;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_x > 319) mouse_x = 319;
    if (mouse_y > 199) mouse_y = 199;

    mouse_left = (packet[0] & 0x01) ? 1 : 0;
    mouse_right = (packet[0] & 0x02) ? 1 : 0;
    return 1;
}

int mouse_get_x() { return mouse_x; }
int mouse_get_y() { return mouse_y; }
int mouse_left_button() { return mouse_left; }
int mouse_right_button() { return mouse_right; }
