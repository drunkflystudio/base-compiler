
enable_warnings()

set(src
    main.c
    )

source_group("Source Files" FILES ${src})

add_executable(CompilerTests ${src})
target_link_libraries(CompilerTests LuaParser FlyCompilerCore)
set_target_properties(CompilerTests PROPERTIES
    FOLDER "Tests"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/.."
    OUTPUT_NAME "t_compil"
    )

register_test_runner(CompilerTests t_compil)
