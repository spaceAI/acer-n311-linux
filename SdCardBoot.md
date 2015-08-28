# Introduction #
We need to place root filesystem on SD/MMC card in two cases:

  1. When kernel is big enough (more than 1.7Mb), it fails to mount initrd. This seems to be the bootloader issue.
  1. We want **to run complete system** with GUI

## Formatting ##
Format your SD card and **make 2 partitions**:

  1. **FAT partition** for storing HaRET, initrd and kernel.
  1. **Ext2 partition** for root filesystem.

Then **transfer the required files** to the rootfs partition:

> sudo tar -jxvf rootfs.tar.bz2 -C path/to/second/partition
**or**
> sudo tar -zxvf rootfs.tar.gz -C path/to/second/partition
**or**
> sudo cp -pR path/to/rootfs path/to/second/partition


You can transfer the contents of initrd.img.gz to the card. To do this, unarchive the file, mount it, and copy as above.

> mount -o loop initrd.img /mnt/initrd