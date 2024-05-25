# run the kernel #
qemu-system-i386 -drive id=ata0.0,file=build/bootdisk.img,format=raw
