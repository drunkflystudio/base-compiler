
if(PACK_TEST_SCRIPT)
    file(READ "${in}" input HEX)
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," output "${input}")
    file(WRITE "${out}" "static const char ${name}[] = { ${output} };\n")
    return()
endif()

######################################################################################################################

set(tests
    l_main.lua
    l_num1.lua
    l_num8.lua
    l_num10.lua
    l_num16.lua
    p_attr.lua
    p_enum.lua
    p_main.lua
    p_types.lua
    )

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
set(testlist_h "${CMAKE_CURRENT_BINARY_DIR}/testlist.h")

set(gen)
set(includes)
set(code)

foreach(test ${tests})
    get_filename_component(name "${test}" NAME)
    get_filename_component(name_we "${test}" NAME_WE)
    set(in "${CMAKE_CURRENT_SOURCE_DIR}/${test}")
    set(h "${CMAKE_CURRENT_BINARY_DIR}/${name}.h")
    add_custom_command(OUTPUT "${h}"
        COMMAND "${CMAKE_COMMAND}" ^
            -DPACK_TEST_SCRIPT=TRUE ^
            "-Din=${in}" ^
            "-Dout=${h}" ^
            "-Dname=${name_we}" ^
            -P "${CMAKE_CURRENT_LIST_FILE}"
        DEPENDS "${in}" "${CMAKE_CURRENT_LIST_FILE}"
        MAIN_DEPENDENCY "${in}"
        )
    list(APPEND gen "${h}")
    set(includes "${includes}#include \"${h}\"\n")
    set(code "${code}runTest(L, \"=${in}\", ${name_we}, sizeof(${name_we}) - 1);\n")
endforeach()

list(LENGTH gen count)

maybe_write_file("${testlist_h}" "{\n${includes}${code}}\n")

######################################################################################################################

enable_warnings()

set(gen
    "${CMAKE_CURRENT_BINARY_DIR}/conf.h"
    ${gen}
    )

set(src
    main.c
    parsercb.h
    test.c
    tests.h
    util.c
    conf-in.h
    )

source_group("Source Files" FILES ${src} ${tests})
source_group("Generated Files" FILES ${gen} "${testlist_h}")

if(WIN32)
    set(DIFF_EXE "${PROJECT_DIR}/_tools/gnuwin32/diff.exe")
else()
    unset(DIFF_EXE)
endif()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/conf-in.h"
    "${CMAKE_CURRENT_BINARY_DIR}/conf.h")

add_executable(CompilerTests ${src} ${gen} ${tests} "${testlist_h}")
target_include_directories(CompilerTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(CompilerTests LuaParser FlyCompilerCore)
set_target_properties(CompilerTests PROPERTIES
    FOLDER "Tests"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/.."
    OUTPUT_NAME "t_compil"
    )

register_test_runner(CompilerTests t_compil)
add_dependencies(RunCompilerTests RunBaseTests)
