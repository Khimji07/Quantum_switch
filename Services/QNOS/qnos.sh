
mkdir ../rootfs/common/etc/sv/qnos
cp -vr qnos/* ../rootfs/common/etc/sv/qnos
cd ../rootfs/common/etc/runit/runsvdir/default/
ln -s /etc/sv/qnos qnos
echo "Service QNOS installed Successfully"