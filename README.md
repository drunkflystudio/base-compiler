
Fly compiler
============

This is a compiler for the Fly language used to develop the platform.

Prerequisites
-------------

To build the compiler you will need:

* CMake 3.10
* Visual Studio 2022 on Windows (optional)

All other dependencies are downloaded automatically.

Building on Windows
-------------------

To build compiler on Windows make sure CMake is on PATH and then execute:

* `build_win32.cmd` (will fail if you don't have Visual Studio installed)
* or, optionally, one of the `win32_*.cmd` files in the `cmake` directory.

Executables will be placed in the `build` directory.
