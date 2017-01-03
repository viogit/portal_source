#!/bin/sh
export CC="x86_64-w64-mingw32-gcc"
export WINDRES="x86_64-w64-mingw32-windres"
export PLATFORM=mingw32
export ARCH=x86_64
exec make $*
sleep 1000
