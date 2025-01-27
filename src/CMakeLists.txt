
enable_warnings()

#######################################################################################################################
## BtYacc

set(parser_y "${CMAKE_CURRENT_SOURCE_DIR}/drunkfly/compiler/parser.y")
set(parser_c "${CMAKE_CURRENT_BINARY_DIR}/y.tab.c")
set(parser_h "${CMAKE_CURRENT_BINARY_DIR}/y.tab.h")

add_custom_command(OUTPUT "${parser_c}" "${parser_h}"
    COMMAND "${HOST_BTYACC}" -d -y "${parser_y}"
    DEPENDS "${parser_y}" "${HOST_BTYACC}"
    MAIN_DEPENDENCY "${parser_y}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )

######################################################################################################################

set(src_compiler
    drunkfly/compiler/arena.c
    drunkfly/compiler/arena.h
    drunkfly/compiler/parser.y
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

######################################################################################################################

set(src
    main.c
    )

source_group("Source Files" FILES ${src})
add_executable(FlyCompiler ${src})
set_target_properties(FlyCompiler PROPERTIES FOLDER "Tools")
target_link_libraries(FlyCompiler PUBLIC FlyCompilerCore)
add_dependencies(FlyCompiler RunCompilerTests)
