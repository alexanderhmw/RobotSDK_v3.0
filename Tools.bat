@echo off

set TMPCURPATH=%cd%

cd /D %~dp0

if not exist .\Build\Tools\NUL mkdir Build\Tools
if not exist .\VS\Tools\NUL mkdir VS\Tools
cd VS\Tools

if not "%1"=="" set PATH=%PATH%;%1; & goto Build
if defined RobotDep_Bin set PATH=%PATH%;%RobotDep_Bin%; & goto PreBuild
if defined QTBINDIR set PATH=%PATH%;%QTBINDIR%; & goto PreBuild


set /p QTBINDIR=Input absolute path of Qt\bin:
set PATH=%PATH%;%QTBINDIR%;

:PreBuild

if exist ..\..\Build\Tools\ConfigSystem.exe goto ConfigSystem

:Build

qmake -tp vc -r ../../Src/Tools/Tools.pro "CONFIG+=build_all"
devenv Tools.sln /build Release

:ConfigSystem
echo Start ConfigSystem.exe
cd ..\..\Build\Tools
ConfigSystem.exe

echo Tools Building Completed!

:ExitBat

cd /D %TMPCURPATH%

pause

@echo on