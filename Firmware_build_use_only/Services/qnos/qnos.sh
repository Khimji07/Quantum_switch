#!/bin/bash

PWD=$(pwd)

directory="$PWD/../rootfs/common/etc/sv/qnos"

if [ -d "$directory" ]; then
    echo "Service "qnos" already Installed."
else
    mkdir ../rootfs/common/etc/sv/qnos
    cp -vr qnos/* ../rootfs/common/etc/sv/qnos
    cd ../rootfs/common/etc/runit/runsvdir/default/
    ln -s /etc/sv/qnos qnos
    echo "Service QNOS installed Successfully"
fi
