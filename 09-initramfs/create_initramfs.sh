#!/bin/bash

ARCH="x86_64"
BB_VER="1.31.0"

# Dirs
mkdir -p root
cd root
mkdir -p bin dev etc lib mnt proc sbin sys tmp var
cd -

# Utils
if [ ! -f "root/bin/busybox" ]; then
    curl -L "https://www.busybox.net/downloads/binaries/${BB_VER}-defconfig-multiarch-musl/busybox-${ARCH}" >root/bin/busybox
fi
cd root/bin
chmod +x busybox
ln -s busybox mount
ln -s busybox sh
cd -

# Init process

cat >>root/init << EOF
#!/bin/busybox sh
/bin/busybox --install -s /bin
mount -t devtmpfs  devtmpfs  /dev
mount -t proc      proc      /proc
mount -t sysfs     sysfs     /sys
mount -t tmpfs     tmpfs     /tmp
setsid cttyhack sh
exec /bin/sh
EOF
chmod +x root/init

# initramfs creation

cd root
find . | cpio -ov --format=newc | gzip -9 >../initramfs
cd -
