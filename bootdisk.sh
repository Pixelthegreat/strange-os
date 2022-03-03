#!/bin/bash

# script for setting up bootdisk

cd build

# copy image
cp ../bootdisk.img ./

# setup loopback device #
sudo losetup -o $[63 * 512] /dev/loop1 ./bootdisk.img

# mount disk image #
sudo mkdir -pv /mnt/strange-os/
sudo mount /dev/loop1 /mnt/strange-os/

# copy kernel
sudo cp ./strange /mnt/strange-os/boot/

# copy grub config
sudo cp ../grub/grub.cfg /mnt/strange-os/boot/grub/

# unmount device
sudo umount /mnt/strange-os/

sudo losetup -D /dev/loop1

# exit
cd ..

exit 0
