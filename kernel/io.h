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

#endif
