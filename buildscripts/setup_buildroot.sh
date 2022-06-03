#!/bin/bash
set -e
set -x
cd /atomtools/build/buildroot-2016.02

rm -rf /atomtools/build/buildroot-2016.02/package/ffmpeg
rm -rf /atomtools/build/buildroot-2016.02/package/mjpg-streamer
rm -rf /atomtools/build/buildroot-2016.02/package/ncurses
cp -r /src/custompackages/package/* /atomtools/build/buildroot-2016.02/package/

patch -p1 < /src/patches/add_fp_no_fused_madd.patch
patch -p1 < /src/patches/libv4l_add_lpthread.patch
patch -p1 < /src/patches/linux_makefile.patch

cp /src/configs/atomcam_defconfig configs/
cp /src/configs/busybox.config package/busybox
make atomcam_defconfig
make savedefconfig BR2_DEFCONFIG=/src/configs/atomcam_defconfig

# mipsel-gcc for uLibc
cd /atomtools/build
git clone https://github.com/Dafang-Hacks/mips-gcc472-glibc216-64bit.git --depth 1

# Start the build process
cd /atomtools/build/buildroot-2016.02
make clean
make
