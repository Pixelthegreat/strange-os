# run the kernel
qemu-system-i386 -device ide-cd,id=ata0 -drive file=build/main.iso,id=ata0.0
