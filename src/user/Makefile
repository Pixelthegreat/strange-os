LD=ld # linker #
CC=cc # c compiler #
ASM=nasm

ASMARCH=elf32
LDARCH=elf_i386

# helper #
CFILENAME=@echo "$(notdir $<)"
LFILENAME=@echo "$(notdir $@)"

# flags #
CFLAGS=-c -m32 -ffreestanding -fno-pie -nostdlib -nostartfiles -nodefaultlibs -Isrc/include -static -DSTDLIB
ASMFLAGS=-f$(ASMARCH)
LDFLAGS=build/header.o build/stdlib.a -T src/user/linker.ld -m$(LDARCH)

all: header init sh echo ls cat

# simple header to load entry point #
header: src/user/header.asm
	$(CFILENAME)
	$(ASM) $(ASMFLAGS) -o build/header.o src/user/header.asm

# init program #
init: build/init.o
	$(LFILENAME)
	$(LD) -o build/user/init build/init.o $(LDFLAGS)

build/init.o: src/user/init.c
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/init.o src/user/init.c

# tty shell #
sh: build/sh.o
	$(LFILENAME)
	$(LD) -o build/user/sh build/sh.o $(LDFLAGS)

build/sh.o: src/user/sh.c
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/sh.o src/user/sh.c

# echo #
echo: build/echo.o
	$(LFILENAME)
	$(LD) -o build/user/echo build/echo.o $(LDFLAGS)

build/echo.o: src/user/echo.c
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/echo.o src/user/echo.c

# ls #
ls: build/ls.o
	$(LFILENAME)
	$(LD) -o build/user/ls build/ls.o $(LDFLAGS)

build/ls.o: src/user/ls.c
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/ls.o src/user/ls.c

# cat #
cat: build/cat.o
	$(LFILENAME)
	$(LD) -o build/user/cat build/cat.o $(LDFLAGS)

build/cat.o: src/user/cat.c
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/cat.o src/user/cat.c
