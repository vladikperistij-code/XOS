#include "time.h"
#include "gui.h"

// Порти для роботи з CMOS
#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71

// Допоміжна функція для читання байта з порту (нам знадобиться asm)
unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void port_byte_out(unsigned short port, unsigned char data) {
    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}

unsigned char get_update_in_progress_flag() {
    port_byte_out(CMOS_ADDR, 0x0A);
    return (port_byte_in(CMOS_DATA) & 0x80);
}

unsigned char get_rtc_register(int reg) {
    port_byte_out(CMOS_ADDR, reg);
    return port_byte_in(CMOS_DATA);
}

// Перетворення BCD формату в звичайний індетифікатор
#define BCD2BIN(bcd) ((((bcd) & 0xF0) >> 4) * 10 + ((bcd) & 0x0F))

void get_time(int *h, int *m, int *s) {
    while (get_update_in_progress_flag());
    *s = BCD2BIN(get_rtc_register(0x00));
    *m = BCD2BIN(get_rtc_register(0x02));
    *h = BCD2BIN(get_rtc_register(0x04));
}

// Проста реалізація sleep без таймера PIT — просто порожній цикл
// 1.0 приблизно дорівнює великій кількості ітерацій
void sleep(int seconds) {
    for(long i = 0; i < seconds * 100000000; i++) {
        __asm__("nop"); // порожня операція
    }
}
