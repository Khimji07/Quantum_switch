# AC3

# mkdir ../rootfs/ac3/etc/sv/livelogs
# cp -vr livelogs/* ../rootfs/ac3/etc/sv/livelogs/
# cd ../rootfs/ac3/etc/runit/runsvdir/default/
# ln -s /etc/sv/livelogs livelogs


# A2

mkdir ../rootfs/a2/etc/sv/livelogs
cp -vr livelogs/* ../rootfs/a2/etc/sv/livelogs
cd ../rootfs/a2/etc/runit/runsvdir/default/
ln -s /etc/sv/livelogs livelogs