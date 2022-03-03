# strange-os
A Unix-Like Operating System kernel written in C.

## idea
I was working on an operating system before this. However, it was using the FAT12 filesystem (which was probably not a filesystem that meets the standards of a UNIX-Like OS), and there were generally a number of other downsides as well, making me choose this option instead.

## update
I added some EXT2 support, but it isn't fully finished yet.

## build and run
To build, make sure you are in the top-level directory and type ```make```. This should build everything so that it can be ready to run. Next, type ```./run.sh```. This will open an instance of qemu, with the built bootdisk for strange-os.

### requirements (for building and running)
- a c compiler
- nasm
- qemu (or bochs, or really any other x86 pc emulator)

### note for older commits
if needing to build an older version based off of commits 0-6, please make sure to create the ```build``` directory before actually building.
