@echo off
cd /D "%~dp0.." || exit /B 1
cmake ^
    -DBORLAND_4_5=debug;release ^
    -DHTML5=debug;release ^
    -DLINUX64=debug;release ^
    -DMINGW810_32=debug;release ^
    -DMINGW810_64=debug;release ^
    -DMSVC2022_32=debug;release ^
    -DMSVC2022_64=debug;release ^
    -DWATCOM10=debug;release ^
    -P "%~dp0buildtool/buildtool.cmake" || exit /B 1
echo == SUCCESS! ==
