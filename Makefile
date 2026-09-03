ASM = nasm
CC = gcc
LD = ld

CFLAGS = -m32 -std=gnu99 -ffreestanding -fno-stack-protector -fno-pie -nostdlib -Wall -Wextra -O2 -Iinclude
LDFLAGS = -m elf_i386 -nostdlib -T linker.ld

OBJS = build/kernel_entry.o build/switch.o build/kernel.o build/vga.o build/keyboard.o build/process.o build/scheduler.o

all: seng21213-os.img

build/kernel_entry.o: kernel/kernel_entry.asm
	@mkdir -p build
	$(ASM) -f elf32 $< -o $@

build/switch.o: boot/switch.asm
	@mkdir -p build
	$(ASM) -f elf32 $< -o $@

build/kernel.o: kernel/kernel.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/vga.o: kernel/vga.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/keyboard.o: kernel/keyboard.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/process.o: kernel/process.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/scheduler.o: kernel/scheduler.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/kernel.elf: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) $(OBJS) -o build/kernel.elf

build/kernel.bin: build/kernel.elf
	objcopy -O binary build/kernel.elf build/kernel.bin

seng21213-os.img: build/kernel.bin boot/boot.bin
	dd if=/dev/zero bs=512 count=2880 of=seng21213-os.img 2>/dev/null
	dd if=boot/boot.bin of=seng21213-os.img conv=notrunc bs=512 count=1 2>/dev/null
	dd if=build/kernel.bin of=seng21213-os.img conv=notrunc bs=512 seek=1 2>/dev/null
	@echo "✓ Build successful -> seng21213-os.img"

run: seng21213-os.img
	qemu-system-i386 -fda seng21213-os.img -m 32M

clean:
	rm -rf build seng21213-os.img

.PHONY: all run clean
