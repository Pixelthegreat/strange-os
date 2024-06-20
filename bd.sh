#!/bin/sh

# script for setting up bootdisk

cd build

# copy image
cp ../bootdisk.img ./

# setup loopback device #
looppart=`sudo kpartx -l bootdisk.img | awk -e '{ print $1; exit }'`
loopdev=`sudo kpartx -l bootdisk.img | awk -e '{ print $5; exit }'`
sudo kpartx -a bootdisk.img

# mount disk image #
mkdir -pv ../tmp
sudo mount "/dev/mapper/$looppart" ../tmp

sudo mkdir -pv ../tmp/boot
sudo mkdir -pv ../tmp/boot/grub
sudo cp -rnv ../grub/system/* ../tmp
sudo cp -rnv user/* ../tmp/bin

# copy kernel
sudo cp -v ./strange ../tmp/boot/

# copy grub config
sudo cp -v ../grub/grub.cfg ../tmp/boot/grub/

# unmount device
sudo umount ../tmp

sudo kpartx -d bootdisk.img

# exit
cd ..

exit 0
