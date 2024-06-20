#!/bin/sh
# bootdisk generation script for strange-os #

MODULES='boot biosdisk part_msdos fat configfile normal multiboot2'

echo generating file...
dd if=/dev/zero of=bootdisk.img count=1 bs=16M

echo generating partition table...
sudo parted --script bootdisk.img mklabel msdos mkpart p fat32 1 16 set 1 boot on

# map disk image #
echo mapping partitions...

looppart=`sudo kpartx -l bootdisk.img | awk -e '{ print $1; exit }'`
loopdev=`sudo kpartx -l bootdisk.img | awk -e '{ print $5; exit }'`
sudo kpartx -a bootdisk.img
sleep 1

# setup fat32 fs #
echo generating fs...
#sudo mkfs.fat "/dev/mapper/$looppart" -F 16 -D 41 -f 2 -h 0 --mbr -r 224 -R 1 -s 4 -S 512
sudo mkfs.fat "/dev/mapper/$looppart" -F 32

# mount partition #
echo mounting partition...
mkdir -pv tmp
sudo mount "/dev/mapper/$looppart" tmp

# copy files #
echo copying system files...
sudo cp -rv grub/system/* tmp
sudo mkdir -pv tmp/boot
sudo mkdir -pv tmp/boot/grub
sudo cp -v grub/grub.cfg tmp/boot/grub
sudo cp -v grub/device.map tmp/boot/grub

# install grub #
echo installing grub...
echo "(hd0) $loopdev" > grub/device.map
sudo grub-install --no-floppy --grub-mkdevicemap=grub/device.map --root-directory=tmp --boot-directory=tmp/boot --install-modules="$MODULES" --modules="$MODULES" $loopdev

# install syslinux #
#echo installing syslinux...
#sudo syslinux -f -d tmp "/dev/mapper/$looppart"

# unmount #
echo unmounting...
sudo umount tmp

# unmap #
echo unmapping...
sudo kpartx -d bootdisk.img

echo done.
