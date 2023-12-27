#!/bin/bash

PWD=$(pwd)

directory="$PWD/../rootfs/common/etc/sv/tunnel"

if [ -d "$directory" ]; then
    echo "Service "tunnel" already Installed."
else
    mkdir ../rootfs/common/etc/sv/tunnel
    cp -vr tunnel/* ../rootfs/common/etc/sv/tunnel
    cd ../rootfs/common/etc/runit/runsvdir/default/
    ln -s /etc/sv/tunnel tunnel
    echo "Service Tunnel installed successfully"
fi