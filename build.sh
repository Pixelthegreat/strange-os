LD=ld # linker
CC=cc # c compiler
ASM=nasm # assembler

# flags
CCFLAGS=" -m32 -ffreestanding -fno-pie -nostdlib -nostartfiles -nodefaultlibs"
ASMFLAGS=" -felf32"
LDFLAGS=" -T ../src/kernel/linker.ld -melf_i386"

# compile and assemble kernel sections
cd src/kernel
$CC $CCFLAGS -o ../../build/kernel.o -c kernel.c
$CC $CCFLAGS -o ../../build/kprint.o -c kprint.c
$ASM $ASMFLAGS -o ../../build/loader.o loader.asm

# link kernel sections (important: loader.o must come BEFORE kernel.o!!!)
cd ../../build
$LD $LDFLAGS -o kernel.bin loader.o kernel.o kprint.o
cd ..
