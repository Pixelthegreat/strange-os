# strange-os
A Unix-Like Operating System kernel written in C.

## idea
I was working on an operating system before this. However, it was using the FAT12 filesystem (which was probably not a filesystem that meets the standards of a UNIX-Like OS), and there were generally a number of other downsides as well, making me choose this option instead.

## update
The update of September 17th, 2021 FINALLY adds support for reading from IDE drives. This took me awhile, as you can probably tell, but I am super happy now that it is done. As for the driver, it is based off of the one from "xv6", another OS project which is a recreation of Unix System 6. You can find that [here](https://github.com/mit-pdos/xv6-public/). The system supports both reading and writing. For it to work, you currently need the IDE device that you're using to be attached to ATA0 Master.

## build and run
To build, make sure you are in the top-level directory and type ```sh build.sh```. This should build everything so that it can be ready to run. Next, type ```sh run.sh```. This will open an instance of qemu.

### requirements (for building and running)
- a c compiler
- nasm
- qemu (or bochs, or really any other x86 pc emulator)

### note for older commits
if needing to build an older version based off of commits 0-6, please make sure to create the ```build``` directory before actually building.
