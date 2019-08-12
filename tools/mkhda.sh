#!/bin/sh
dd if=/dev/zero of=hda.img bs=1024 count=1024
mkfs.ext2 -F -I128 -b1024 hda.img
dd if=boot/boot of=hda.img bs=512 count=1 conv=notrunc
sudo mount hda.img /mnt
sudo cp -r base/* /mnt
sudo cp Image /mnt
sudo umount /mnt
