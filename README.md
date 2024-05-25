# strange-os
A Unix-Like Operating System kernel written in C.

## update
I've updated a lot of things recently, so I will go over them briefly:
1. I changed the filesystem used from ext2 to FAT32 (ext2 is fine overall but it has a lot of annoying nuance).
2. I rewrote the vga text driver to use 320x200 graphics mode (the graphics mode part might've been unnecessary, but I needed to rewrite the driver anyway and I thought it would be fun).
3. I rewrote the heap.
4. I reworte the paging system.

I rewrote most of these things because of major issues that prevented development of other parts of the kernel. For these cases, I felt it was just better to rewrite said features in a more structured manner than previously, and I believe that has turned out good so far.

## build and run
The OS (at the moment) can only be built on x86 or x86-64 Linux as far as I am aware. To generate the bootdisk, run `./genbootdisk.sh`. To build the kernel and install it onto the bootdisk, run `make`. To run, run `./run.sh`.

Considering how far WSL 2 has come, I wouldn't be surprised if the build also worked there, in which case it is also possible to build on Windows. However, I have never personally tested this before.

NOTE FOR GENERATING BOOTDISK: The bootdisk script uses the `grub-install` command, which on my system installs Grub 2. However, your system might have a `grub2-install` command instead, in which case I would recommend changing the beginning of line 39 in `genbootdisk.sh` to `sudo grub2-install`.

### requirements (for building and running)
- gcc
- kpartx
- parted
- mkfs.fat
- nasm
- qemu
