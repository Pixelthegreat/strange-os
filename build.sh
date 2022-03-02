#!/bin/bash
# create the build directory if it doesn't exist
mkdir -pv build

LD=ld # linker
CC=cc # c compiler
ASM=nasm # assembler

# flags
CCFLAGS=" -m32 -ffreestanding -fno-pie -nostdlib -nostartfiles -nodefaultlibs"
ASMFLAGS=" -felf32"
LDFLAGS=" -T ../src/kernel/linker.ld -melf_i386"

echo building kernel...

# compile and assemble kernel sections
cd src/kernel
echo kernel.o
$CC $CCFLAGS -o ../../build/kernel.o -c kernel.c
echo kprint.o
$CC $CCFLAGS -o ../../build/kprint.o -c kprint.c
echo idt.o
$CC $CCFLAGS -o ../../build/idt.o -c idt.c
echo util.o
$CC $CCFLAGS -o ../../build/util.o -c util.c
echo gdt.o
$CC $CCFLAGS -o ../../build/gdt.o -c gdt.c
echo syscall.o
$CC $CCFLAGS -o ../../build/syscall.o -c syscall.c
echo ata.o
$CC $CCFLAGS -o ../../build/ata.o -c disk/ata.c
echo ext2.o
$CC $CCFLAGS -o ../../build/ext2.o -c fs/ext2.c
echo page.o
$CC $CCFLAGS -o ../../build/page.o -c page.c
echo panic.o
$CC $CCFLAGS -o ../../build/panic.o -c panic.c
echo mbr.o
$CC $CCFLAGS -o ../../build/mbr.o -c disk/mbr.c
echo dev.o
$CC $CCFLAGS -o ../../build/dev.o -c dev.c
echo heap.o
$CC $CCFLAGS -o ../../build/heap.o -c heap.c
echo fs.o
$CC $CCFLAGS -o ../../build/fs.o -c fs/fs.c

# libc base
echo unistd.o
$CC $CCFLAGS -o ../../build/unistd.o -c io/unistd.c
echo string.o
$CC $CCFLAGS -o ../../build/string.o -c util/string.c

# kernel loader
echo loader.o
$ASM $ASMFLAGS -o ../../build/loader.o loader.asm
cd ../..

# compile multiboot stuff
cd src/boot
echo multiboot.o
$ASM $ASMFLAGS -o ../../build/multiboot.o multiboot.asm
echo boot0.o
gcc -D__asm -g -m32 -o ../../build/boot0.o -c boot.S
echo stage2.o
gcc -D__asm -g -m32 -o ../../build/stage2.o -c stage2.S
echo a20.o
gcc -D__asm -g -m32 -o ../../build/a20.o -c a20.S
echo stage2.asm.o
$ASM $ASMFLAGS -o ../../build/stage2.asm.o stage2.asm
echo boot0.bin
ld -T boot.ld -o ../../build/boot0.elf ../../build/boot0.o ../../build/stage2.o ../../build/a20.o ../../build/stage2.asm.o
objcopy -O binary ../../build/boot0.elf ../../build/boot0.bin
cd ../..

# link kernel sections (important: multiboot.o and loader.o must come BEFORE kernel.o!!!)
cd build
echo strange \(kernel\)
$LD $LDFLAGS -o strange multiboot.o loader.o kernel.o kprint.o idt.o gdt.o util.o unistd.o string.o syscall.o ata.o ext2.o page.o panic.o mbr.o dev.o heap.o fs.o

echo setting up files on disk...

# copy disk image file
cp ../bootdisk.img ./

# setup loopback device for grub #
sudo losetup -o $[63 * 512] /dev/loop1 ./bootdisk.img

# mount disk image
sudo mkdir -pv /mnt/strange-os/
sudo mount /dev/loop1 /mnt/strange-os/

echo strange
# copy kernel
sudo cp ./strange /mnt/strange-os/boot/

echo grub.cfg
# copy grub config
sudo cp ../grub/grub.cfg /mnt/strange-os/boot/grub

# unmount drive
sudo umount /mnt/strange-os/

sudo losetup -D /dev/loop1

# exit
cd ..

# remove kernel from grub folder
#rm -r grub/boot
