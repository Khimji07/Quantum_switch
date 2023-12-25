mkdir ../rootfs/common/etc/sv/genral
cp -vr genral/* ../rootfs/common/etc/sv/genral
cd ../rootfs/common/etc/runit/runsvdir/default/
ln -s /etc/sv/genral genral
echo "Service genral installed successfully"
