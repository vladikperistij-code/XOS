# ==============================================================================
# 🚀 XOS v1.3 Professional Makefile [XTechnologies]
# ==============================================================================

# Інструменти
CC = gcc
AS = nasm
LD = ld

# Налаштування компіляції (32-біт, без ліб, інклюди для всіх папок)
CFLAGS = -m32 -ffreestanding -fno-stack-protector -fno-pic -nostdlib -Ikernel -Ixfs -Iapps -w -c
LDFLAGS = -m elf_i386 -Ttext 0x1000 --entry main --oformat binary

# Папки
BIN = bin
KDIR = kernel
DDIR = kernel/drivers
XDIR = xfs
ADIR = apps
BDIR = boot

# Об'єктні файли (kernel.o має бути першим!)
OBJ = $(BIN)/kernel.o $(BIN)/gui.o $(BIN)/time.o $(BIN)/keyboard.o \
      $(BIN)/power.o $(BIN)/xfs.o $(BIN)/ata.o \
      $(BIN)/xeditor.o $(BIN)/xrun.o $(BIN)/xstat.o $(BIN)/xcalc.o

# Резервуємо 128 секторів (64КБ) для образу
FULL_SIZE = 65536

# ------------------------------------------------------------------------------
# ЦІЛІ (TARGETS)
# ------------------------------------------------------------------------------

# Головна команда: спочатку збірка, потім запуск
all: build run

# Очищення та повна перезбірка
clean_all: clean build run

# Створення образу диска
build: $(BIN)/xos.bin
	@echo "--- [BUILD COMPLETE] ---"

$(BIN)/xos.bin: $(BIN)/boot.bin $(BIN)/kernel.bin
	@echo "--- [CREATING XOS.BIN] ---"
	cat $(BIN)/boot.bin $(BIN)/kernel.bin > $(BIN)/xos.bin
	truncate -s $(FULL_SIZE) $(BIN)/xos.bin

# Запуск в QEMU з підтримкою звуку (сумісно з Windows/WSL)
run: $(BIN)/xos.bin
	@echo "--- [STARTING QEMU] ---"
	qemu-system-i386 -machine pc,pcspk-audiodev=snd0 -m 256M \
	-drive format=raw,file=$(BIN)/xos.bin,index=0,if=ide \
	-audiodev $(shell (qemu-system-i386 -audiodev help | grep -q dsound && echo dsound) || echo sdl),id=snd0

# --- ПРАВИЛА ЗБІРКИ ---

$(BIN)/boot.bin: $(BDIR)/boot.asm
	@mkdir -p $(BIN)
	$(AS) -f bin $< -o $@

$(BIN)/kernel.bin: $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $@

# Універсальні правила для .c файлів
$(BIN)/%.o: $(KDIR)/%.c
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/%.o: $(DDIR)/%.c
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/%.o: $(XDIR)/%.c
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/%.o: $(ADIR)/%.c
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(BIN)
