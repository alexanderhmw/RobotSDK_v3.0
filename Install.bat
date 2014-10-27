@echo off

set "TMPCURPATH=%cd%"

set "TMPDISKDRIVER="
set "QTDIR="
set "BOOSTDIR="

if defined RobotSDK_Kernel set "TMPDISKDRIVER=%RobotSDK_Kernel:~0,1%" & goto ConfigRobotSDK
if not "%1"=="" set "TMPDISKDRIVER=%1" & goto ConfigRobotSDK

set /p TMPDISKDRIVER=Input disk driver for RobotSDK (default c):

:ConfigRobotSDK

set "RobotSDK_Kernel=%TMPDISKDRIVER%:\SDK\RobotSDK\Kernel"
set "RobotSDK_Tools=%TMPDISKDRIVER%:\SDK\RobotSDK\Tools"

:ConfigQt
 
if not "%2"=="" set "QTDIR=%2"
if not "%3"=="" set "BOOSTDIR=%3"

if defined RobotDep_Bin set "PATH=%PATH%;%RobotDep_Bin%;" & goto InstallRobotSDK

if defined QTDIR goto ConfigBoost
set /p QTDIR=Input path of Qt (contains bin, include and lib):

:ConfigBoost

if defined BOOSTDIR goto ConfigRobotDep
set /p BOOSTDIR=Input libXX-msvc-XX.X path of Boost:

:ConfigRobotDep

set "RobotDep_Include=%QTDIR%\include;%BOOSTDIR%\..;"
set "RobotDep_Lib=%QTDIR%\lib;%BOOSTDIR%;"
set "RobotDep_Bin=%QTDIR%\bin;%BOOSTDIR%;"
set "PATH=%PATH%;%RobotDep_Bin%;"

:InstallRobotSDK

echo Start Building Tools!

cd /D %~dp0

if not exist .\Build\Tools\NUL mkdir .\Build\Tools
if not exist .\VS\Tools\NUL mkdir .\VS\Tools
cd .\VS\Tools

qmake -tp vc -r ../../Src/Tools/Tools.pro "CONFIG+=build_all"
devenv Tools.sln /build Release

xcopy ..\..\Src\Tools\ConfigModule\*.pri ..\..\Build\Tools\ /s /e /y

echo Start ConfigSystem.exe

cd ..\..\Build\Tools

if defined QTDIR if defined BOOSTDIR start .\ConfigSystem.exe %TMPDISKDRIVER% %QTDIR% %BOOSTDIR% & goto FinishConfigSystem
if defined QTDIR start .\ConfigSystem.exe %TMPDISKDRIVER% %QTDIR% & goto FinishConfigSystem
start .\ConfigSystem.exe %TMPDISKDRIVER%

:FinishConfigSystem

echo Tools Building Completed!

echo Start Building Kernel!

cd /D %~dp0

if not exist .\Build\Kernel\include\NUL mkdir .\Build\Kernel\include
if not exist .\Build\Kernel\lib\NUL mkdir .\Build\Kernel\lib
if not exist .\VS\Kernel\NUL mkdir .\VS\Kernel
cd .\VS\Kernel

qmake -tp vc ../../Src/Kernel/Kernel.pro "CONFIG+=build_all"
devenv Kernel.vcxproj /build Release
devenv Kernel.vcxproj /build Debug

xcopy ..\..\Src\Kernel\*.h ..\..\Build\Kernel\include\ /s /e /y
xcopy ..\..\Src\Kernel\*.xml ..\..\Build\Kernel\include\ /s /e /y

echo Kernel Building Completed!

echo Start Generating Documentation!

cd /D %~dp0\Doc\Doxygen

doxygen RobotSDK

echo Documentation Generation Completed!

echo Start Installing!

cd /D %~dp0

if not exist .\Build\Tools\*.exe echo Tools are not built! & goto ExitBat

set "ToolPath=%RobotSDK_Tools:/=\%"
if not exist %ToolPath%\NUL mkdir %ToolPath%

xcopy .\Build\Tools\*.exe %ToolPath% /s /e /y
xcopy .\Build\Tools\*.pri %ToolPath% /s /e /y

if not exist %ToolPath%\*.xml xcopy .\Build\Tools\* %ToolPath% /s /e /y

set "KernelPath=%RobotSDK_Kernel:/=\%"
if not exist %KernelPath%\NUL mkdir %KernelPath%

xcopy .\Build\Kernel\* %KernelPath% /s /e /y

if not exist .\Doc\html\NUL echo Documentation is not compiled! & goto Finish
if not exist %KernelPath%\..\Doc\NUL mkdir %KernelPath%\..\Doc
xcopy .\Doc\html\* %KernelPath%\..\Doc /s /e /y

:Finish

echo Installation Completed!

:ExitBat

cd /D %TMPCURPATH%

@echo on
