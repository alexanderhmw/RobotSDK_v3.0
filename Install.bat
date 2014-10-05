@echo off

set TMPCURPATH=%cd%

cd /D %~dp0

if not exist .\Build\Tools\*.exe echo Tools are not built! & goto ExitBat

if not defined RobotSDK_Tools echo RobotSDK_Tools is not defined! & goto ExitBat

if not defined RobotSDK_Kernel echo RobotSDK_Kernel is not defined! & goto ExitBat

rem Install Tools

set ToolPath=%RobotSDK_Tools:/=\%
if not exist %ToolPath%\NUL mkdir %ToolPath%

xcopy .\Build\Tools\*.exe %ToolPath% /s /e /y
if not exist %ToolPath%\*.xml xcopy .\Build\Tools\* %ToolPath% /s /e /y

set KernelPath=%RobotSDK_Kernel:/=\%

if not exist %KernelPath%\NUL mkdir %KernelPath%
xcopy .\Build\Kernel\* %KernelPath% /s /e /y

echo Installation Completed!

:ExitBat

cd /D %TMPCURPATH%

pause

@echo on

exit