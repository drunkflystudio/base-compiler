cmake_minimum_required(VERSION 3.5...3.31)
project(Compiler)

# If this line fails, you are building this project wrong.
# Clone <https://github.com/drunkflystudio/drunkflystudio> and follow instructions there.
include(../_cmake/common.cmake)

add_subdirectory(src)
add_subdirectory(tests)
