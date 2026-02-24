#ifndef ATA_H
#define ATA_H

#include "../kernel/io.h" // Тут уже є inb, outb, outw

// Додамо тільки inw, якщо його немає в io.h
#ifndef INW_DEFINED
#define INW_DEFINED
static inline unsigned short __attribute__((always_inline)) inw(unsigned short port) {
    unsigned short ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
#endif

#define ATA_DATA        0x1F0
#define ATA_SECCOUNT    0x1F2
#define ATA_LBA_LOW     0x1F3
#define ATA_LBA_MID     0x1F4
#define ATA_LBA_HIGH    0x1F5
#define ATA_DRIVE_SEL   0x1F6
#define ATA_COMMAND     0x1F7
#define ATA_STATUS      0x1F7

void ata_read_sector(int lba, unsigned short* buffer);
void ata_write_sector(int lba, unsigned short* buffer);

#endif
