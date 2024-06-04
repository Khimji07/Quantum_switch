#!/bin/bash

# TOP_DIR=${TOP_DIR}
# PACKAGES_DIR=${PACKAGES_DIR}
# SDK_SOURCES=${SDK_SOURCES}

#
# Common
lib_extract_packages() {
    echo "> ${FUNCNAME}"
    cd ${TOP_DIR}

    # NOTE: these must be pulled as job artifacts!
    rm -rf ${PACKAGES_DIR} && mkdir ${PACKAGES_DIR}
    for _ar in ${REPLICA_DIR}/output/${REPLICA_TARGET}/*.tar.bz2; do tar -xf ${_ar} -C ${PACKAGES_DIR}; done
}

lib_install_packages() {
    echo "> ${FUNCNAME}"
    cd ${SDK_DIR}

    # TODO: remove snmpget dependency?
    cp    ${PACKAGES_DIR}/usr/bin/{miyagi,rpg,jq,snmpget} ./rootfs/usr/bin
    cp -r ${PACKAGES_DIR}/www                             ./rootfs

    cp -r ${PACKAGES_DIR}/etc/skel/miyagi ./rootfs/etc/skel/
    if [ $PLATFORM == 'SW225-CAT3' ]; then
        cp -r ${TOP_DIR}/QN_PKGS_AC3/build/*         ./rootfs/usr/bin/
    elif [ $PLATFORM == 'SW225-A2' ]; then
        cp -r ${TOP_DIR}/QN_PKGS_A2/build/*         ./rootfs/usr/bin/
    fi

    mkdir -p ./rootfs/tmp/qnos
    mkdir -p ./rootfs/etc/miyagi
    touch ./rootfs/etc/miyagi/configuration.json
}

lib_prepare_rootfs() {
    echo "> ${FUNCNAME}"
    cd ${SDK_DIR}

    cp -ar ../rootfs/common/* ./rootfs

    # TODO: check packages.sh for changes
    sed -i "s+^root:[^:]*:+root:!disabled:+" ./rootfs/etc/passwd
}

lib_verify_rootfs() {
    echo "> ${FUNCNAME}"
    cd ${SDK_DIR}

    gunzip --stdout ./kernel/usr/initramfs_data.cpio.gz | cpio -tv
}

lib_extract_mibs() {
    mkdir -p ./rootfs/usr/share/snmp
    tar -xf ${TOP_DIR}/sdk/${MIBS_ARCHIVE} -C ./rootfs/usr/share/snmp
    # NOTE: rlvoipsnoop.mib has an error in its definition and rfc2011.mib is deprecated
    rm ./rootfs/usr/share/snmp/mibs/{rlvoipsnoop,rfc2011}.mib
}

#
# AlleyCat3 and PonCat3 specific
libcat3_extract_toolchain() {
    echo "> ${FUNCNAME}"
    cd ${TOP_DIR}

    # NOTE: this is compiler running on 32bit (multilib) x86!
    rm -rf ${TOP_DIR}/armv7-marvell-linux-gnueabi
    tar -xf ${TOP_DIR}/sdk/armv7-marvell-linux-gnueabi.tar.gz
}

libcat3_prepare_buildenv() {
    echo "> ${FUNCNAME}"
    cd ${SDK_DIR}

    sed -i "/CONFIG_INITRAMFS_ROOT_UID/ s|0|$(id -u)|g" ./Makefile
    sed -i "/CONFIG_INITRAMFS_ROOT_GID/ s|1|$(id -g)|g" ./Makefile
    sed -i '/^YYLTYPE/ s/^/extern /' ./kernel/scripts/dtc/dtc-lexer.lex.c
    sed -i "/# CROSS_TOOLCHAIN=/a CROSS_TOOLCHAIN=${PWD}/../armv7-marvell-linux-gnueabi" ./create_ros_image.sh
}

libcat3_configure_buildenv() {
    echo "> ${FUNCNAME}"
    cd ${SDK_DIR}

    local DROPBEAR_VERSION="2020.81"

    sed -i '/\Wconfigure_init/ s/^/#/' ./packages.sh
    sed -i "/strip --strip-unneeded/ s/-type f/-type f ! -name 'qnhelperbox' ! -name 'tunnel'/" ./packages.sh
    sed -in '/SCRIPT_NAME/q;p' ./packages/dropbear/build.sh

    # FIXME: this is only a temp solution to dropbear SHA1 removal
    cd ./packages/dropbear
    tar -xjf dropbear-${DROPBEAR_VERSION}.tar.bz2
    rm dropbear-${DROPBEAR_VERSION}.tar.bz2
    sed -i '/define DROPBEAR_SHA1_HMAC/ s/1$/0/' dropbear-${DROPBEAR_VERSION}/default_options.h
    sed -i '/define DROPBEAR_SHA1_96_HMAC/ s/1$/0/' dropbear-${DROPBEAR_VERSION}/default_options.h
    sed -i '/define DROPBEAR_DH_GROUP14_SHA1/ s/1$/0/' dropbear-${DROPBEAR_VERSION}/default_options.h
    tar -cjf dropbear-${DROPBEAR_VERSION}.tar.bz2 dropbear-${DROPBEAR_VERSION}
    rm -rf dropbear-${DROPBEAR_VERSION}
    cd ${SDK_DIR}

    sed -in '/SCRIPT_NAME/q;p' ./packages/dnsresolv/feature.sh
    chmod -x ./packages/telnetd/feature.sh

    # required for connecting to unix sockets (AF_UNIX)
    sed -i "/FEATURE_NSLOOKUP_BIG/a CONFIG_FEATURE_UNIX_LOCAL=y" ./packages/busybox/user-config.txt
}

libcat3_prepare_rootfs() {
    echo "> ${FUNCNAME}"
    cd ${SDK_DIR}

    # NOTE: we leave out --force/-f flag to allow for verbose failure
    rm -r  ./rootfs/var/log
    cp -ar ../rootfs/cat3/* ./rootfs

    # NOTE: we won't be using this init system
    rm -r ./rootfs/etc/init.d
    rm -r ./rootfs/etc/rc3.d
    rm    ./rootfs/etc/rc
}

#
# Aldrin2 specific
liba2_extract_toolchain() {
    echo "> ${FUNCNAME}"
    cd ${TOP_DIR}

    rm -rf ${TOP_DIR}/gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu
    tar -xf ${TOP_DIR}/sdk/gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu.tar.xz
}

liba2_prepare_buildenv() {
    echo "> ${FUNCNAME}"
    cd ${SDK_DIR}

    sed -i "/# CROSS_TOOLCHAIN=/a CROSS_TOOLCHAIN=${PWD}/../gcc-arm-8.3-2019.03-x86_64-aarch64-linux-gnu" ./create_ros_image.sh
    sed -i 's/make zImage_ramfs/yes "" | make zImage_ramfs/' ./create_ros_image.sh
}

liba2_configure_buildenv() {
    echo "> ${FUNCNAME}"
    cd ${SDK_DIR}

    local DROPBEAR_VERSION="2020.81"

    sed -i '/\Wconfigure_init/ s/^/#/' ./packages.sh
    sed -i "/strip --strip-unneeded/ s/-type f/-type f ! -name 'qnhelperbox' ! -name 'tunnel'/" ./packages.sh
    sed -in '/SCRIPT_NAME/q;p' ./packages/dropbear/build.sh

    # FIXME: this is only a temp solution to dropbear SHA1 removal
    cd ./packages/dropbear
    tar -xjf dropbear-${DROPBEAR_VERSION}.tar.bz2
    rm dropbear-${DROPBEAR_VERSION}.tar.bz2
    sed -i '/define DROPBEAR_SHA1_HMAC/ s/1$/0/' dropbear-${DROPBEAR_VERSION}/default_options.h
    sed -i '/define DROPBEAR_SHA1_96_HMAC/ s/1$/0/' dropbear-${DROPBEAR_VERSION}/default_options.h
    sed -i '/define DROPBEAR_DH_GROUP14_SHA1/ s/1$/0/' dropbear-${DROPBEAR_VERSION}/default_options.h
    tar -cjf dropbear-${DROPBEAR_VERSION}.tar.bz2 dropbear-${DROPBEAR_VERSION}
    rm -rf dropbear-${DROPBEAR_VERSION}
    cd ${SDK_DIR}

    sed -in '/SCRIPT_NAME/q;p' ./packages/dnsresolv/feature.sh
    chmod -x ./packages/telnetd/feature.sh

    # required for connecting to unix sockets (AF_UNIX)
    sed -i "/FEATURE_NSLOOKUP_BIG/a CONFIG_FEATURE_UNIX_LOCAL=y" ./packages/busybox/user-config.txt
    sed -i '$aCONFIG_FEATURE_DEVPTS=n' ./packages/busybox/user-config.txt
}

liba2_prepare_rootfs() {
    echo "> ${FUNCNAME}"
    cd ${SDK_DIR}

    # NOTE: use busybox init
    rm ./rootfs/sbin/init
    ln -s busybox ./rootfs/sbin/init
    rm -r ./rootfs/etc/init.d
    rm -r ./rootfs/etc/rc3.d
    rm    ./rootfs/etc/rc
    rm    ./rootfs/etc/rc.common

    # NOTE: we leave out --force/-f flag to allow for verbose failure
    rm     ./rootfs/etc/resolv.conf
    rm -r  ./rootfs/var/log
    cp -ar ../rootfs/a2/* ./rootfs
}
