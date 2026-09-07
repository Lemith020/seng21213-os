ASM_OBJECTS = build/kernel_entry.o build/switch.o
C_OBJECTS = build/kernel.o build/vga.o build/keyboard.o build/process.o build/scheduler.o build/thread.o build/mutex.o build/semaphore.o build/pmm.o build/ramdisk.o build/fs.o

all: seng21213-os.img

seng21213-os.img: build/kernel.bin
	dd if=/dev/zero bs=512 count=2880 of=seng21213-os.img 2>/dev/null
	dd if=boot/boot.bin of=seng21213-os.img conv=notrunc bs=512 count=1 2>/dev/null
	dd if=build/kernel.bin of=seng21213-os.img conv=notrunc bs=512 seek=1 2>/dev/null
	@echo "✓ Build successful -> seng21213-os.img"

build/kernel.bin: $(ASM_OBJECTS) $(C_OBJECTS)
	ld -m elf_i386 -T linker.ld -o build/kernel.bin $(ASM_OBJECTS) $(C_OBJECTS) --oformat binary

build/kernel_entry.o: kernel/kernel_entry.asm
	mkdir -p build
	nasm -f elf32 kernel/kernel_entry.asm -o build/kernel_entry.o

build/switch.o: boot/switch.asm
	mkdir -p build
	nasm -f elf32 boot/switch.asm -o build/switch.o

build/kernel.o: kernel/kernel.c
	mkdir -p build
	gcc -m32 -std=gnu99 -ffreestanding -fno-stack-protector -fno-pie -nostdlib -Wall -Wextra -O2 -Iinclude -c kernel/kernel.c -o build/kernel.o

build/vga.o: kernel/vga.c
	mkdir -p build
	gcc -m32 -std=gnu99 -ffreestanding -fno-stack-protector -fno-pie -nostdlib -Wall -Wextra -O2 -Iinclude -c kernel/vga.c -o build/vga.o

build/keyboard.o: kernel/keyboard.c
	mkdir -p build
	gcc -m32 -std=gnu99 -ffreestanding -fno-stack-protector -fno-pie -nostdlib -Wall -Wextra -O2 -Iinclude -c kernel/keyboard.c -o build/keyboard.o

build/process.o: kernel/process.c
	mkdir -p build
	gcc -m32 -std=gnu99 -ffreestanding -fno-stack-protector -fno-pie -nostdlib -Wall -Wextra -O2 -Iinclude -c kernel/process.c -o build/process.o

build/scheduler.o: kernel/scheduler.c
	mkdir -p build
	gcc -m32 -std=gnu99 -ffreestanding -fno-stack-protector -fno-pie -nostdlib -Wall -Wextra -O2 -Iinclude -c kernel/scheduler.c -o build/scheduler.o

build/thread.o: kernel/thread.c
	mkdir -p build
	gcc -m32 -std=gnu99 -ffreestanding -fno-stack-protector -fno-pie -nostdlib -Wall -Wextra -O2 -Iinclude -c kernel/thread.c -o build/thread.o

build/mutex.o: kernel/mutex.c
	mkdir -p build
	gcc -m32 -std=gnu99 -ffreestanding -fno-stack-protector -fno-pie -nostdlib -Wall -Wextra -O2 -Iinclude -c kernel/mutex.c -o build/mutex.o

build/semaphore.o: kernel/semaphore.c
	mkdir -p build
	gcc -m32 -std=gnu99 -ffreestanding -fno-stack-protector -fno-pie -nostdlib -Wall -Wextra -O2 -Iinclude -c kernel/semaphore.c -o build/semaphore.o

build/pmm.o: kernel/pmm.c
	mkdir -p build
	gcc -m32 -std=gnu99 -ffreestanding -fno-stack-protector -fno-pie -nostdlib -Wall -Wextra -O2 -Iinclude -c kernel/pmm.c -o build/pmm.o

build/ramdisk.o: kernel/ramdisk.c
	mkdir -p build
	gcc -m32 -std=gnu99 -ffreestanding -fno-stack-protector -fno-pie -nostdlib -Wall -Wextra -O2 -Iinclude -c kernel/ramdisk.c -o build/ramdisk.o

build/fs.o: kernel/fs.c
	mkdir -p build
	gcc -m32 -std=gnu99 -ffreestanding -fno-stack-protector -fno-pie -nostdlib -Wall -Wextra -O2 -Iinclude -c kernel/fs.c -o build/fs.o

run: seng21213-os.img
	qemu-system-i386 -fda seng21213-os.img -m 32M

clean:
	rm -rf build seng21213-os.img
