
# 1. Create raw image.
dd if=/dev/zero of=uefi.img bs=512 count=93750

# 2. Create UEFI partition on the image.
gdisk uefi.img <<EOF
o
Y
n



ef00
w
Y
EOF

# 3. Format into the FAT32 file system. 
sudo losetup --offset 1048576 --sizelimit 46934528 /dev/loop99 uefi.img
sudo mkdosfs -F 32 /dev/loop99
sudo losetup -d /dev/loop99
