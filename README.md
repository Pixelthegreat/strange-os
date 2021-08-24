# strange-os
A Unix-Like Operating System kernel written in C.

## idea
I was working on an operating system before this. However, it was using the FAT12 filesystem (which was probably not a filesystem that meets the standards of a UNIX-Like OS), and there were generally a number of other downsides as well, making me choose this option instead.

## update
The update of August 24, 2021 adds some base kernel implementations of ```unistd.h``` functions, as well as a basic file buffer system. It is still under construction, although the implemented functions work properly (with stdin, and stdout; full file support won't be added until I can gain functionality to read sectors from ATA devices, and build upon that). The update also adds a GDT and IDT. The IDT will see it's own custom syscall vector implementation within sometime, once I am able to load programs that need to show some sort of output to prove that they are working.

## build and run
To build, make sure you are in the top-level directory and type ```sh build.sh```. This should build everything so that it can be ready to run. Next, type ```sh run.sh```. This will open an instance of qemu.

### requirements (for building and running)
- a c compiler
- nasm
- qemu (or bochs, or really any other x86 pc emulator)

### note for older commits
if needing to build an older version based off of commits 0-6, please make sure to create the ```build``` directory before actually building.
