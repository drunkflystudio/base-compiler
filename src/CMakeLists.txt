
enable_warnings()

#######################################################################################################################
## RE2C

set(re2c_flags
    --api simple
    --bit-vectors
    --empty-class error
    --encoding-policy fail
    --input-encoding utf8
    --lang c
    --nested-ifs
    --utf8
    -W
    )
if(MSVC)
    set(re2c_flags ${re2c_flags} --location-format msvc)
else()
    set(re2c_flags ${re2c_flags} --location-format gnu)
endif()
if(CMAKE_COMPILER_IS_GNUCC)
    set(re2c_flags ${re2c_flags}
        --case-ranges
        #--computed-gotos
        )
endif()

set(lexer_re "${CMAKE_CURRENT_SOURCE_DIR}/drunkfly/compiler/lexer.re")
set(lexer_c "${CMAKE_CURRENT_BINARY_DIR}/lexer.c")

add_custom_command(OUTPUT "${lexer_c}"
    COMMAND "${HOST_RE2C}" ${re2c_flags} --output "${lexer_c}" "${lexer_re}"
    DEPENDS "${lexer_re}" "${HOST_RE2C}"
    MAIN_DEPENDENCY "${lexer_re}"
    )

#######################################################################################################################
## BtYacc

set(parser_y "${CMAKE_CURRENT_SOURCE_DIR}/drunkfly/compiler/parser.y")
set(parser_ske "${CMAKE_CURRENT_SOURCE_DIR}/drunkfly/compiler/parser.ske")
set(parser_c "${CMAKE_CURRENT_BINARY_DIR}/y.tab.c")
set(parser_h "${CMAKE_CURRENT_BINARY_DIR}/y.tab.h")

add_custom_command(OUTPUT "${parser_c}" "${parser_h}"
    COMMAND "${HOST_BTYACC}" -d -S "${parser_ske}" -y "${parser_y}"
    DEPENDS "${parser_y}" "${parser_ske}" "${HOST_BTYACC}"
    MAIN_DEPENDENCY "${parser_y}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )

######################################################################################################################

set(src_compiler
    drunkfly/compiler/arena.c
    drunkfly/compiler/arena.h
    drunkfly/compiler/bundle.c
    drunkfly/compiler/bundle.h
    drunkfly/compiler/codegen.c
    drunkfly/compiler/codegen.h
    drunkfly/compiler/lexer.h
    drunkfly/compiler/lexer.re
    drunkfly/compiler/parser.ske
    drunkfly/compiler/parser.y
    drunkfly/compiler/private.h
    drunkfly/compiler/semantic.c
    drunkfly/compiler/semantic.h
    drunkfly/compiler/token.h
    )

set(src
    drunkfly/compiler.c
    drunkfly/compiler.h
    )

set(gen
    "${lexer_c}"
    "${parser_c}"
    "${parser_h}"
    )

source_group("Source Files\\drunkfly" FILES ${src})
source_group("Source Files\\drunkfly\\compiler" FILES ${src_compiler})
source_group("Generated Files" FILES ${gen})
add_library(FlyCompilerCore STATIC ${src} ${src_compiler} ${gen})

set_target_properties(FlyCompilerCore PROPERTIES FOLDER "Libs")
target_include_directories(FlyCompilerCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(FlyCompilerCore PUBLIC FlyCommon)

######################################################################################################################

set(src_compiler
    drunkfly/compiler/cg_boot.c
    )

source_group("Source Files\\drunkfly\\compiler" FILES ${src_compiler})
add_library(FlyCompilerBackendBootstrap STATIC ${src_compiler})

set_target_properties(FlyCompilerBackendBootstrap PROPERTIES FOLDER "Libs")
target_link_libraries(FlyCompilerBackendBootstrap PUBLIC FlyCompilerCore)

######################################################################################################################

set(src
    main.c
    )

source_group("Source Files" FILES ${src})

if(NOT CMAKE_CROSSCOMPILING)
    set(FlyCompiler FlyCompiler)
else()
    set(FlyCompiler FlyCompilerBuild)
    add_executable(FlyCompiler IMPORTED GLOBAL)
    set_target_properties(FlyCompiler PROPERTIES IMPORTED_LOCATION "${HOST_FLYCC}")
endif()

add_executable("${FlyCompiler}" ${src})
target_link_libraries("${FlyCompiler}" PUBLIC FlyCompilerCore FlyCompilerBackendBootstrap)
add_dependencies("${FlyCompiler}" RunCompilerTests)

set_target_properties("${FlyCompiler}" PROPERTIES
    FOLDER "Tools"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/.."
    OUTPUT_NAME "flycc"
    )
