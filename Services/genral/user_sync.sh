#!/bin/bash

PWD=$(pwd)

directory="$PWD/../rootfs/common/etc/sv/genral"

if [ -d "$directory" ]; then
    echo "Service "General" already Installed."
else
    mkdir ../rootfs/common/etc/sv/genral
    cp -r genral/* ../rootfs/common/etc/sv/genral
    cd ../rootfs/common/etc/runit/runsvdir/default/
    ln -s /etc/sv/genral genral
    echo "Service genral installed successfully"
fi
