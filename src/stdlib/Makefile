AR=ar # archiver #
CC=cc # c compiler #
ASM=nasm # assembler #

ASMARCH=elf32

# helper #
CFILENAME=@echo "$(notdir $<)"
LFILENAME=@echo "$(notdir $@)"

# flags #
CFLAGS=-c -m32 -ffreestanding -fno-pie -nostdlib -nostartfiles -nodefaultlibs -DSTDLIB
ASMFLAGS=-f$(ASMARCH)

OBJFILES=build/stdlib/syscall.o build/stdlib/starch.o build/stdlib/string.o build/stdlib/stdio.o build/stdlib/crt.o

all: stdlib

stdlib: $(OBJFILES)
	$(LFILENAME)
	$(AR) cr build/stdlib.a $(OBJFILES)

build/stdlib/syscall.o: src/stdlib/syscall.asm src/include/_syscall.h
	$(CFILENAME)
	$(ASM) $(ASMFLAGS) -o build/stdlib/syscall.o src/stdlib/syscall.asm

build/stdlib/starch.o: src/stdlib/starch.c src/include/starch.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/stdlib/starch.o src/stdlib/starch.c

build/stdlib/string.o: src/stdlib/string.c src/include/string.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/stdlib/string.o src/stdlib/string.c

build/stdlib/stdio.o: src/stdlib/stdio.c src/include/stdio.h
	$(CFILENAME)
	$(CC) $(CFLAGS) -o build/stdlib/stdio.o src/stdlib/stdio.c

build/stdlib/crt.o: src/stdlib/crt.asm
	$(CFILENAME)
	$(ASM) $(ASMFLAGS) -o build/stdlib/crt.o src/stdlib/crt.asm
