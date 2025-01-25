
set(src_compiler
    drunkfly/compiler/arena.c
    drunkfly/compiler/arena.h
    drunkfly/compiler/private.h
    drunkfly/compiler/token.h
    )

set(src
    drunkfly/compiler.c
    drunkfly/compiler.h
    )

source_group("Source Files\\drunkfly" FILES ${src})
source_group("Source Files\\drunkfly\\compiler" FILES ${src_compiler})
add_library(FlyCompilerCore STATIC ${src} ${src_compiler})

set_target_properties(FlyCompilerCore PROPERTIES FOLDER "Libs")
target_include_directories(FlyCompilerCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(FlyCompilerCore PUBLIC FlyCommon)
