#!/bin/bash
export PSPDEV=/usr/local/pspdev
export PATH=$PATH:$PSPDEV/bin
cd /mnt/d/3s-psp

# Clean build artifacts but preserve resources/ junction
if [ -d build ]; then
    find build -maxdepth 1 ! -name build ! -name resources -exec rm -rf {} +
    rm -rf build/CMakeFiles
fi
mkdir -p build
cd build
psp-cmake ..
make -j$(nproc) 2>&1
