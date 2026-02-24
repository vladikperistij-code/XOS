# ==========================================
# XTechnologies - XOS Build System v0.3
# ==========================================

# Інструменти
CC = gcc
AS = nasm
LD = ld

# Папки
BOOT_DIR = boot
KERNEL_DIR = kernel
DRIVERS_DIR = kernel/drivers
BIN_DIR = bin

# Прапорці компиляції
CFLAGS = -m32 -ffreestanding -fno-stack-protector -fno-pic -nostdlib -Ikernel -w -c
LDFLAGS = -m elf_i386 -Ttext 0x1000 --entry main --oformat binary

# Список усіх об'єктних файлів ядра
OBJ = $(BIN_DIR)/kernel.o \
      $(BIN_DIR)/gui.o \
      $(BIN_DIR)/time.o \
      $(BIN_DIR)/keyboard.o \
      $(BIN_DIR)/power.o

# Головна ціль
all: $(BIN_DIR)/xos.bin

# Створення папки bin
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# 1. Завантажувач
$(BIN_DIR)/boot.bin: $(BOOT_DIR)/boot.asm | $(BIN_DIR)
	$(AS) -f bin $(BOOT_DIR)/boot.asm -o $(BIN_DIR)/boot.bin

# 2. Основні модулі ядра
$(BIN_DIR)/kernel.o: $(KERNEL_DIR)/kernel.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $(KERNEL_DIR)/kernel.c -o $(BIN_DIR)/kernel.o

$(BIN_DIR)/gui.o: $(KERNEL_DIR)/gui.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $(KERNEL_DIR)/gui.c -o $(BIN_DIR)/gui.o

$(BIN_DIR)/time.o: $(KERNEL_DIR)/time.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $(KERNEL_DIR)/time.c -o $(BIN_DIR)/time.o

# 3. Драйвери (Keyboard та Power)
$(BIN_DIR)/keyboard.o: $(DRIVERS_DIR)/keyboard.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $(DRIVERS_DIR)/keyboard.c -o $(BIN_DIR)/keyboard.o

$(BIN_DIR)/power.o: $(DRIVERS_DIR)/power.c | $(BIN_DIR)
	$(CC) $(CFLAGS) $(DRIVERS_DIR)/power.c -o $(BIN_DIR)/power.o

# 4. Лінкування ядра
$(BIN_DIR)/kernel.bin: $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(BIN_DIR)/kernel.bin

# 5. Фінальний образ ОС
$(BIN_DIR)/xos.bin: $(BIN_DIR)/boot.bin $(BIN_DIR)/kernel.bin
	cat $(BIN_DIR)/boot.bin $(BIN_DIR)/kernel.bin > $(BIN_DIR)/xos.bin
	truncate -s +64k $(BIN_DIR)/xos.bin

# Команда запуску
run: all
	qemu-system-i386 -machine pc -m 256M -drive format=raw,file=$(BIN_DIR)/xos.bin

# Очищення
clean:
	rm -rf $(BIN_DIR)
