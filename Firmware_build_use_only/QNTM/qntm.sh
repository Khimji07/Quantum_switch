#!/bin/bash

PWD=$(pwd)

directory="$PWD/../rootfs/common/etc/QNTM_Serial"

if [ -d "$directory" ]; then
    echo "Service "QNTM_Serial" already Installed."
else
    mkdir ../rootfs/common/etc/QNTM_Serial
    cp -r QNTM_serial/* ../rootfs/common/etc/QNTM_Serial
    
    echo "Service QNTM_Serial installed successfully"
fi
