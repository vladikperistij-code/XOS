#ifndef IO_H
#define IO_H

static inline unsigned char __attribute__((always_inline)) inb(unsigned short port) {
    unsigned char reg;
    __asm__ volatile ("inb %1, %0" : "=a"(reg) : "Nd"(port));
    return reg;
}

static inline void __attribute__((always_inline)) outb(unsigned short port, unsigned char data) {
    __asm__ volatile ("outb %b0, %w1" : : "a"(data), "Nd"(port));
}

static inline void __attribute__((always_inline)) outw(unsigned short port, unsigned short data) {
    __asm__ volatile ("outw %w0, %w1" : : "a"(data), "Nd"(port));
}

// ... твої існуючі inb, outb, outw ...
static inline void play_sound(unsigned int nFrequence) {
    unsigned int Div = 1193180 / nFrequence;
    outb(0x43, 0xb6);
    outb(0x42, (unsigned char) (Div) );
    outb(0x42, (unsigned char) (Div >> 8));
    unsigned char tmp = inb(0x61);
    if (tmp != (tmp | 3)) outb(0x61, tmp | 3);
}

static inline void nosound() {
    unsigned char tmp = inb(0x61) & 0xFC;
    outb(0x61, tmp);
}

#endif