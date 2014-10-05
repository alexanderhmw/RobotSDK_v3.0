@echo off

set TMPCURPATH=%cd%

cd /D %~dp0

if not exist .\Build\Kernel\include\NUL mkdir Build\Kernel\include
if not exist .\Build\Kernel\lib\NUL mkdir Build\Kernel\lib
if not exist .\VS\Kernel\NUL mkdir VS\Kernel
cd VS\Kernel

if not defined RobotDep_Bin echo RobotDep_Bin is not defined! & goto ExitBat

set PATH=%PATH%;%RobotDep_Bin%;

qmake -tp vc ../../Src/Kernel/Kernel.pro "CONFIG+=build_all"
devenv Kernel.vcxproj /build Release
devenv Kernel.vcxproj /build Debug

xcopy ..\..\Src\Kernel\*.h ..\..\Build\Kernel\include\ /s /e /y
xcopy ..\..\Src\Kernel\*.xml ..\..\Build\Kernel\include\ /s /e /y

echo Kernel Building Completed!

:ExitBat

cd /D %TMPCURPATH%

pause

@echo on

exit