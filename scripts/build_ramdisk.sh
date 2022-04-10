#!/bin/bash
cd FatFsMaker
make
cd ..
./FatFsMaker/out/fatfs_maker 67108864 ramdisk.img ramdisk_pack
