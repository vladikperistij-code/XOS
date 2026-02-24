# Налаштування інструментів
CC = gcc
AS = nasm
LD = ld

# Прапорці компиляції (32-біт, без стандартних ліб, інклюд ядра)
CFLAGS = -m32 -ffreestanding -fno-stack-protector -fno-pic -nostdlib -Ikernel -Ixfs -w -c
LDFLAGS = -m elf_i386 -Ttext 0x1000 --entry main --oformat binary

# Папки проєкту
BIN = bin
KDIR = kernel
DDIR = kernel/drivers
XDIR = xfs
BDIR = boot

# Список усіх об'єктних файлів
OBJ = $(BIN)/kernel.o \
      $(BIN)/gui.o \
      $(BIN)/time.o \
      $(BIN)/keyboard.o \
      $(BIN)/power.o \
      $(BIN)/xfs.o \
      $(BIN)/ata.o


run: $(BIN)/xos.bin
	qemu-system-i386 -m 256M -drive format=raw,file=bin/xos.bin,index=0,media=disk

# Головна команда для термінала
all: clean $(BIN)/xos.bin run

# Складання фінального образу ОС
$(BIN)/xos.bin: $(BIN)/boot.bin $(BIN)/kernel.bin
	cat $(BIN)/boot.bin $(BIN)/kernel.bin > $(BIN)/xos.bin
	# Робимо образ достатнього розміру для тестів файлової системи (1МБ)
	truncate -s 1M $(BIN)/xos.bin

# Bootloader (asm)
$(BIN)/boot.bin: $(BDIR)/boot.asm
	mkdir -p $(BIN)
	$(AS) -f bin $< -o $@

# Лінковка ядра (всі .o файли в один .bin)
$(BIN)/kernel.bin: $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $@

# --- Компіляція модулів ядра ---
$(BIN)/kernel.o: $(KDIR)/kernel.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/gui.o: $(KDIR)/gui.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/time.o: $(KDIR)/time.c
	$(CC) $(CFLAGS) $< -o $@

# --- Компіляція драйверів ---
$(BIN)/keyboard.o: $(DDIR)/keyboard.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/power.o: $(DDIR)/power.c
	$(CC) $(CFLAGS) $< -o $@

# --- Компіляція XFS (у папці xfs/) ---
$(BIN)/xfs.o: $(XDIR)/xfs.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/ata.o: $(XDIR)/ata.c
	$(CC) $(CFLAGS) $< -o $@

# Очищення проекту
clean:
	rm -rf $(BIN)

# Швидкий перезапуск без очищення
rerun: $(BIN)/xos.bin run
