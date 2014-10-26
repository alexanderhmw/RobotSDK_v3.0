@echo off

set TMPCURPATH=%cd%

cd /D %~dp0\..\Doc\Doxygen

doxygen RobotSDK

echo Documentation Generation Completed!

cd /D %TMPCURPATH%

@echo on