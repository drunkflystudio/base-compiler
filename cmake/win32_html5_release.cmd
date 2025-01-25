@echo off
cd /D "%~dp0.." || exit /B 1
cmake -DHTML5=release -P "%~dp0buildtool/buildtool.cmake" || exit /B 1
echo == SUCCESS! ==
