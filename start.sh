#!/bin/sh

appimagetool-url=https://github.com/AppImage/AppImageKit/releases/download/13/appimagetool-x86_64.AppImage
baseAppImage=$(ls | grep -i '\.AppImage$' | head -n 1)
baseAppImagePath=$(readlink -f $baseAppImage)

# Build navicat patcher
echo "Building Navicat Patcher..."
make all

# Extract AppImage
echo "Extracting AppImage..."
mkdir -p /navicat_mount /navicat
mount -o loop $baseAppImagePath /navicat_mount
cp -r /navicat_mount/* /navicat
umount /navicat_mount
echo "Extracted AppImage."

# Start Patcher
echo "Starting Patcher..."
/patcher/bin/navicat-patcher /navicat
echo "Navicat Patched."

# Repack
echo "Repacking AppImage..."
wget -O appimagetool.AppImage 'https://github.com/AppImage/AppImageKit/releases/download/13/appimagetool-x86_64.AppImage'
chmod +x appimagetool.AppImage
./appimagetool.AppImage /navicat /patcher/bin/$(echo $baseAppImage | sed 's/\.AppImage$/-patched.AppImage/')
echo "Repacked AppImage."

# Start Keygen
echo "Starting Keygen... (REMEMBER TO DISCONNECT INTERNET)"
read -p "Press ENTER to continue"
/patcher/bin/navicat-keygen --text ./RegPrivateKey.pem
echo "Copy activation code and continue activa
echo "Exiting..."
exit 0