# How to set up default.txt

# Example of default.txt for n311 #

set ramaddr 0x30000000

set ramsize 0x04000000

set kernel "zImage" # Fill the correct filename

#set initrd "initrd.img.gz" # Choose it if you use /dev/ram as root (not recommended)

set mtype 1178 # 1248 for the kernels after Dec 22 2008.

set bootspd 0

# Using SD Card as root

set cmdline "root=/dev/mmcblk0p**_partitionnumber_** init=/sbin/init rootdelay=1"

#set cmdline "root=/dev/ram init=/sbin/init" # May not work with big kernels

bootlinux