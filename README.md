# android-dumptool
Memory search/dump utils for Android

To build these tools, you'll need the Android NDK and ndk-build in the path.

There are two utils created in the build:
- dumpmem - Maps /dev/mem over a given address range (hardcoded), and searches it.
- dumptool - Uses the Linux ptrace mechanism to search a given process' memory. This may be faster for more targeted searching. 
