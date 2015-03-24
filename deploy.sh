#!/bin/sh

# Build
ndk-build

# Push
adb push libs/armeabi/dumptool /data
adb push libs/armeabi/dumpmem /data

