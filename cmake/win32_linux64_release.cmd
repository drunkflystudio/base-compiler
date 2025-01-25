@echo off
cd /D "%~dp0.." || exit /B 1
cmake -DLINUX64=release -P "%~dp0buildtool/buildtool.cmake" || exit /B 1
echo == SUCCESS! ==
