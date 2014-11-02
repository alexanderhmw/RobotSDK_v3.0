#!/bin/sh
TMPBASEDIR=$(dirname $0);
sudo apt-get -y install qt5-default libboost-all-dev doxygen;
mkdir -p $HOME/Build/RobotSDK/Tools;cd $HOME/Build/RobotSDK/Tools;
qmake -makefile $(TMPBASEDIR)/Src/Tools/Tools.pro -r "CONFIG+=release";
make;
make install;
mkdir -p $HOME/Build/RobotSDK/Kernel;cd $HOME/Build/RobotSDK/Kernel;
qmake -makefile $(TMPBASEDIR)/Src/Kernel/Kernel.pro -r "CONFIG+=build_all";
make -f Makefile.Release;
make -f Makefile.Release install;
make -f Makefile.Debug;
make -f Makefile.Debug install;
cd $(TMPBASEDIR)/Doc/Doxygen;
doxygen RobotSDK;
cd $(TMPBASEDIR)/Doc/html;
mkdir -p $HOME/SDK/RobotSDK/Doc;
cp * $HOME/SDK/RobotSDK/Doc;
