@echo off
cd /D "%~dp0.." || exit /B 1
cmake -DWATCOM10=debug -P "%~dp0buildtool/buildtool.cmake" || exit /B 1
echo == SUCCESS! ==
