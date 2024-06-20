LD=ld # linker #
CC=cc # c compiler #
ASM=nasm # assembler #

ASMARCH=elf32
LDARCH=elf_i386

# helper #
CFILENAME=@echo "$(notdir $<)"
LFILENAME=@echo "$(notdir $@)"

# flags #
CFLAGS=-c -m32 -ffreestanding -fno-pie -nostdlib -nostartfiles -nodefaultlibs -O0
ASMFLAGS=-f$(ASMARCH)
LDFLAGS=-T src/kernel/linker.ld -m$(LDARCH) -O0

OBJFILES=build/multiboot.o build/loader.o build/kernel.o build/kprint.o build/idt.o build/util.o build/syscall.o build/ata.o build/page.o build/panic.o build/mbr.o build/dev.o build/heap.o build/fs.o build/string.o build/vga16.o build/fat.o build/vpal.o build/font.o build/fileio.o build/process.o build/elf.o build/abin.o

all: kernel

kernel: $(OBJFILES) src/kernel/linker.ld
	$(LFILENAME)
	$(LD) $(LDFLAGS) -o build/strange $(OBJFILES)

# main kernel object files #
build/kernel.o: src/kernel/kernel.c
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/kernel.o src/kernel/kernel.c

build/kprint.o: src/kernel/kprint.c src/kernel/kprint.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/kprint.o src/kernel/kprint.c

build/idt.o: src/kernel/idt.c src/kernel/idt.h src/kernel/idt.asm
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/idt.o src/kernel/idt.c

build/util.o: src/kernel/util.c src/kernel/util.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/util.o src/kernel/util.c

build/syscall.o: src/kernel/syscall.c src/kernel/syscall.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/syscall.o src/kernel/syscall.c

build/ata.o: src/kernel/disk/ata.c src/kernel/disk/ata.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/ata.o src/kernel/disk/ata.c

build/page.o: src/kernel/page.c src/kernel/page.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/page.o src/kernel/page.c

build/panic.o: src/kernel/panic.c src/kernel/panic.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/panic.o src/kernel/panic.c

build/mbr.o: src/kernel/disk/mbr.c src/kernel/disk/mbr.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/mbr.o src/kernel/disk/mbr.c

build/dev.o: src/kernel/dev.c src/kernel/dev.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/dev.o src/kernel/dev.c

build/heap.o: src/kernel/heap.c src/kernel/heap.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/heap.o src/kernel/heap.c

build/fs.o: src/kernel/fs/fs.c src/kernel/fs/fs.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/fs.o src/kernel/fs/fs.c

build/vga16.o: src/kernel/video/vga16.c src/kernel/video/vga16.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/vga16.o src/kernel/video/vga16.c

build/string.o: src/kernel/util/string.c src/include/string.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/string.o src/kernel/util/string.c

build/loader.o: src/kernel/loader.asm src/kernel/gdt.asm
	$(CFILENAME)
	$(ASM) $(ASMFLAGS) -o build/loader.o src/kernel/loader.asm

build/multiboot.o: src/boot/multiboot.asm
	$(CFILENAME)
	$(ASM) $(ASMFLAGS) -o build/multiboot.o src/boot/multiboot.asm

build/fat.o: src/kernel/fs/fat.c src/kernel/fs/fat.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/fat.o src/kernel/fs/fat.c

build/vpal.o: src/kernel/video/vpal.c src/kernel/video/vpal.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/vpal.o src/kernel/video/vpal.c

build/font.o: src/kernel/video/font.c src/kernel/video/font.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/font.o src/kernel/video/font.c

build/fileio.o: src/kernel/fileio.c src/kernel/fileio.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/fileio.o src/kernel/fileio.c

build/process.o: src/kernel/process.c src/kernel/process.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/process.o src/kernel/process.c

build/elf.o: src/kernel/elf.c src/kernel/elf.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/elf.o src/kernel/elf.c

build/abin.o: src/kernel/abin.c src/kernel/abin.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/abin.o src/kernel/abin.c
