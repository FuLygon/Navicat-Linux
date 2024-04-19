#!/bin/sh

appimagetoolUrl='https://github.com/AppImage/AppImageKit/releases/download/13/appimagetool-x86_64.AppImage'
baseAppImage=$(ls | grep -i '\.AppImage$' | head -n 1)

echo

# Check if AppImage exists
if [ -z $baseAppImage ]; then
    echo "No AppImage found. Exiting..."
    exit 1
fi
baseAppImagePath=$(readlink -f $baseAppImage)

# User prompt
echo
echo "Detected AppImage: $baseAppImage"
read -p "Start patching with this AppImage? [Y/n] " startScript
if [ $startScript == "n" ] || [ $startScript == "N" ]; then
    echo "Exiting..."
    exit 0
fi

# Build navicat patcher
echo
echo "Building Navicat Patcher..."
make all

# Extract AppImage
echo
echo "Extracting AppImage..."
mkdir -p /navicat_mount /navicat
mount -o loop $baseAppImagePath /navicat_mount
cp -r /navicat_mount/* /navicat
umount /navicat_mount
echo "Extracted AppImage."

# Start Patcher
echo
echo "Starting Patcher..."
/patcher/bin/navicat-patcher /navicat
echo "Navicat Patched."

# Repack
echo
echo "Repacking AppImage..."
wget -O appimagetool.AppImage $appimagetoolUrl
chmod +x appimagetool.AppImage
./appimagetool.AppImage /navicat /patcher/bin/$(echo $baseAppImage | sed 's/\.AppImage$/-patched.AppImage/')
echo "Repacked AppImage."

# Start Keygen
echo
echo "Starting Keygen... (REMEMBER TO DISCONNECT INTERNET)"
read -p "Continue? [Y/n] " startKeygen
if [ $startKeygen == "n" ] || [ $startKeygen == "N" ]; then
    echo "Exiting..."
    exit 0
fi
/patcher/bin/navicat-keygen --text ./RegPrivateKey.pem
echo
echo "Copy activation code and continue activation"
echo "Exiting..."
exit 0