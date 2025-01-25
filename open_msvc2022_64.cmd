@echo off
cd /D "%~dp0" || exit /B 1
if exist "%~dp0build\win64\msvc2022\cmake\Common.sln" goto exist
cmake -DMSVC2022_64=generate-only -P "%~dp0cmake/buildtool/buildtool.cmake" || exit /B 1
:exist
start %~dp0build\win64\msvc2022\cmake\Common.sln || exit /B 1
