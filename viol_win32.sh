#!/bin/sh
export CC="i686-w64-mingw32-gcc"
export WINDRES="i686-w64-mingw32-windres"
export PLATFORM=mingw32
export ARCH=x86
exec make $*
sleep 1000
