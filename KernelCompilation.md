First of all you should be familiar with building kernel for your laptop/desktop.

Then you need a ARM crosscompiler, such as Codesourcery lite http://www.codesourcery.com/sgpp/lite/arm

Get mainline or android kernel source that matches the available n311 patches and patch it

Once you are ready, set proper paths to ar, gcc, and so on in top-level Makefile
and then just

$ make

### Obsolete information ###

Always apply the **disable-uncompressor-putc-hack.patch** from G500. Kernel will not boot without it.