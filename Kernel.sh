#!/bin/sh

apt-get -y install qt5-default libboost-all-dev

cd ..

mkdir Build
cd Build

mkdir gcc
cd gcc

qmake ../../Src/Kernel/Kernel.pro -o Makefile.release -r "CONFIG+=release"
make -f Makefile.release
make -f Makefile.release install

qmake ../../Src/Kernel/Kernel.pro -o Makefile.debug -r "CONFIG+=debug"
make -f Makefile.debug
make -f Makefile.debug install

cd ..
cd ..
rm -r Build
