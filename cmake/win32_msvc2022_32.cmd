@echo off
cd /D "%~dp0.." || exit /B 1
cmake -DMSVC2022_32=debug;release -P "%~dp0buildtool/buildtool.cmake" || exit /B 1
echo == SUCCESS! ==
