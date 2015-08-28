### Overview ###

The **goal** of the project is to create Linux kernel for [Acer n311](http://global.acer.com/products/pda/n300.htm).

Acer n311 is based on [Samsung S3C2440X](http://www.samsung.com/global/business/semiconductor/productInfo.do?fmly_id=229&partnum=S3C2440&&ppmi=PartnoSearch) platform which supports Linux. So the remaining work includes writing machine-specific code and device drivers. Much work has been already done [for E-ten G500](http://www.pierrox.net/G500/) and [for HP RX1950](http://www.handhelds.org/moin/moin.cgi/HpIpaqRx1950) which have very similar design. But the support for touchscreen, buttons and built-in graphics chip [NVidia GoForce 4000](http://www.nvidia.com/page/goforce_4000.html) is crucial for normal usage.
### News ###
The current **2.6.29-n311** kernel enabled experimental **support of audio codec**! Thanks to **polachok** and rx1950 linux team for the driver.

The work on **Wi-Fi** and **Bluetooth** support has been started by **AiFiLTr0**.

Now the kernel has **support for buttons and blue LED**. Thanks to **polachok**!
He's also working hard to get audio codec working.

Since **X works**, you can **try some GUI** on n311!

With initrd from G500 project kernel boots to login prompt. One can log in using the USB connection.

See the hardware support status on the HardwareSupport page.

Some photos can be found on the PhotoPage.

### Screen support ###

Now both frame buffer console and X work fine.

I've written simple frame buffer device driver, so it's now possible to read the kernel messages on the screen. However, there **were** unusual problems with text scrolling and stability(they are gone with the current release).
This [short video on YouTube](http://www.youtube.com/watch?v=_t8jf-LlNyg) represents the state of screen support in deprecated n311-zImage-0.01 version.

### How to run ###
Currently the kernel bootloader is [HaRET](http://www.handhelds.org/moin/moin.cgi/HaRET).
  1. Get **zImage** from the [Downloads section](http://code.google.com/p/acer-n311-linux/downloads/list).
  1. Get **HaRET 0.5.1** or later version.
  1. Angstrom rootfs from rx3000 with **Opie Desktop Environment** is available [ftp://downloads.tuxfamily.org/linuxrx1950/rootfs/Angstrom-opie-image-glibc-test-20070620-rx3000.rootfs.tar.bz2](ftp://downloads.tuxfamily.org/linuxrx1950/rootfs/Angstrom-opie-image-glibc-test-20070620-rx3000.rootfs.tar.bz2). Go to SdCardBoot for instructions.
  1. Prepare **default.txt** according to HaretConfig.
  1. If you **want to connect** through USB, get some knowledge from [these instructions](http://handhelds.org/moin/moin.cgi/EtenG500Howto) and appropriately edit **/etc/network/interfaces** on your host and PDA.
  1. Tap 'Run'.
  1. Enjoy!
### How to develop ###
For the **kernel patches** please refer to [Source section](http://code.google.com/p/acer-n311-linux/source/browse/#svn/trunk).

If you want **to build software** for PDA, it's better you **use QEMU**. Get the armv4l system image from uClibc: http://uclibc.org/downloads/binaries/ and compile the software you need.