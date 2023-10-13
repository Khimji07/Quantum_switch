mkdir ../rootfs/common/etc/sv/tunnel
cp -vr tunnel/* ../rootfs/common/etc/sv/tunnel
cd ../rootfs/common/etc/runit/runsvdir/default/
ln -s /etc/sv/tunnel tunnel
echo "Service Tunnel installed successfully"
