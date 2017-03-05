@echo off

mkdir ga7-mingw
cd ga7-mingw
cmake -G "MinGW Makefiles" -DVIDEO_OPENGLES=0 ../../src/engine
cd ..
