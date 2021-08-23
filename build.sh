# create the build directory if it doesn't exist
mkdir -pv build

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
cd ../..

# compile multiboot stuff
cd src/boot
$ASM $ASMFLAGS -o ../../build/multiboot.o multiboot.asm
cd ../..

# link kernel sections (important: multiboot.o and loader.o must come BEFORE kernel.o!!!)
cd build
$LD $LDFLAGS -o kernel.bin multiboot.o loader.o kernel.o kprint.o

# make directory if it hasn't been
mkdir -pv isofs/boot/grub

# copy kernel and config
cp kernel.bin isofs/boot/
cp ../grub/grub.cfg isofs/boot/grub

# GRand Unified Bootloader 7 3
grub-mkrescue isofs/ -o main.iso

# exit
cd ..

# remove kernel from grub folder
#rm -r grub/boot
