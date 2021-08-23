# strange-os
A Unix-Like Operating System kernel written in C.

## idea
I was working on an operating system before this. However, it was using the FAT12 filesystem (which was probably not a filesystem that meets the standards of a UNIX-Like OS), and there were generally a number of other downsides as well, making me choose this option instead.

## build and run
To build, make sure you are in the top-level directory and type ```sh build.sh```. This should build everything so that it can be ready to run. Next, type ```sh run.sh```. This will open an instance of qemu.

### requirements (for building and running)
- a c compiler
- nasm
- qemu

### note for older commits
if needing to build an older version based off of commits 0-6, please make sure to create the ```build``` directory before actually building.
