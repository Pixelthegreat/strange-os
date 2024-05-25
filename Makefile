LD=ld # linker
CC=cc # c compiler
ASM=nasm # assembler

ASMARCH=elf32
LDARCH=elf_i386

# flags
CFLAGS=-c -m32 -ffreestanding -fno-pie -nostdlib -nostartfiles -nodefaultlibs
ASMFLAGS=-f$(ASMARCH)
LDFLAGS=-T src/kernel/linker.ld -m$(LDARCH)

OBJFILES=build/multiboot.o build/loader.o build/kernel.o build/kprint.o build/idt.o build/util.o build/gdt.o build/syscall.o build/ata.o build/page.o build/panic.o build/mbr.o build/dev.o build/heap.o build/fs.o build/string.o build/vga16.o build/fat.o build/vpal.o build/font.o

# primary targets
all: kernel bootdisk

kernel: $(OBJFILES) src/kernel/linker.ld
	$(LD) $(LDFLAGS) -o build/strange $(OBJFILES)

bootdisk: kernel
	./bd.sh

# main kernel object files
build/kernel.o: src/kernel/kernel.c
	$(CC) $(CFLAGS) -o build/kernel.o src/kernel/kernel.c

build/kprint.o: src/kernel/kprint.c src/kernel/kprint.h
	$(CC) $(CFLAGS) -o build/kprint.o src/kernel/kprint.c

build/idt.o: src/kernel/idt.c src/kernel/idt.h
	$(CC) $(CFLAGS) -o build/idt.o src/kernel/idt.c

build/util.o: src/kernel/util.c src/kernel/util.h
	$(CC) $(CFLAGS) -o build/util.o src/kernel/util.c

build/gdt.o: src/kernel/gdt.c src/kernel/gdt.h
	$(CC) $(CFLAGS) -o build/gdt.o src/kernel/gdt.c

build/syscall.o: src/kernel/syscall.c src/kernel/syscall.h
	$(CC) $(CFLAGS) -o build/syscall.o src/kernel/syscall.c

build/ata.o: src/kernel/disk/ata.c src/kernel/disk/ata.h
	$(CC) $(CFLAGS) -o build/ata.o src/kernel/disk/ata.c

build/ext2.o: src/kernel/fs/ext2.c src/kernel/fs/ext2.h
	$(CC) $(CFLAGS) -o build/ext2.o src/kernel/fs/ext2.c

build/page.o: src/kernel/page.c src/kernel/page.h
	$(CC) $(CFLAGS) -o build/page.o src/kernel/page.c

build/panic.o: src/kernel/panic.c src/kernel/panic.h
	$(CC) $(CFLAGS) -o build/panic.o src/kernel/panic.c

build/mbr.o: src/kernel/disk/mbr.c src/kernel/disk/mbr.h
	$(CC) $(CFLAGS) -o build/mbr.o src/kernel/disk/mbr.c

build/dev.o: src/kernel/dev.c src/kernel/dev.h
	$(CC) $(CFLAGS) -o build/dev.o src/kernel/dev.c

build/heap.o: src/kernel/heap.c src/kernel/heap.h
	$(CC) $(CFLAGS) -o build/heap.o src/kernel/heap.c

build/fs.o: src/kernel/fs/fs.c src/kernel/fs/fs.h
	$(CC) $(CFLAGS) -o build/fs.o src/kernel/fs/fs.c

build/vga16.o: src/kernel/video/vga16.c src/kernel/video/vga16.h
	$(CC) $(CFLAGS) -o build/vga16.o src/kernel/video/vga16.c

build/string.o: src/kernel/util/string.c src/include/string.h
	$(CC) $(CFLAGS) -o build/string.o src/kernel/util/string.c

build/loader.o: src/kernel/loader.asm
	$(ASM) $(ASMFLAGS) -o build/loader.o src/kernel/loader.asm

build/multiboot.o: src/boot/multiboot.asm
	$(ASM) $(ASMFLAGS) -o build/multiboot.o src/boot/multiboot.asm

build/fat.o: src/kernel/fs/fat.c src/kernel/fs/fat.h
	$(CC) $(CFLAGS) -o build/fat.o src/kernel/fs/fat.c

build/vpal.o: src/kernel/video/vpal.c src/kernel/video/vpal.h
	$(CC) $(CFLAGS) -o build/vpal.o src/kernel/video/vpal.c

build/font.o: src/kernel/video/font.c src/kernel/video/font.h
	$(CC) $(CFLAGS) -o build/font.o src/kernel/video/font.c

clean:
	rm build/*.o build/strange
