#!/bin/bash

PWD=$(pwd)

directory="$PWD/../rootfs/common/etc/sv/sysinfo"

if [ -d "$directory" ]; then
    echo "Service "sysinfo" already Installed."
else
    mkdir ../rootfs/common/etc/sv/sysinfo
    cp -r sysinfo/* ../rootfs/common/etc/sv/sysinfo
    cd ../rootfs/common/etc/runit/runsvdir/default/
    ln -s /etc/sv/sysinfo sysinfo
    echo "Service sysinfo installed successfully"
fi
