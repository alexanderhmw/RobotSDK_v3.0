#!/bin/sh
sudo apt-get -y install qt5-default libboost-all-dev doxygen;
mkdir -p GCC/Tools;cd GCC/Tools;
qmake -makefile ../../Src/Tools/Tools.pro -r "CONFIG+=release";
make;
make install;
cd ..;
mkdir -p Kernel;cd Kernel;
qmake ../../Src/Kernel/Kernel.pro -o Makefile.release -r "CONFIG+=release";
make -f Makefile.release;
make -f Makefile.release install;
qmake ../../Src/Kernel/Kernel.pro -o Makefile.debug -r "CONFIG+=debug";
make -f Makefile.debug;
make -f Makefile.debug install;
cd ..;cd ..;cd Doc/Doxygen;
doxygen RobotSDK;
cd ../html;
mkdir -p $HOME/SDK/RobotSDK/Doc;
cp * $HOME/SDK/RobotSDK/Doc;
