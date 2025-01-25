
get_filename_component(SCRIPT_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(PROJECT_DIR "${SCRIPT_DIR}/../..")
set(buildtool_cmake "${PROJECT_DIR}/ext/common/cmake/buildtool/buildtool.cmake")

if(NOT EXISTS "${buildtool_cmake}")
    set(status_code -1)
    execute_process(
        COMMAND git submodule update --init --recursive
        RESULT_VARIABLE status_code
        WORKING_DIRECTORY "${PROJECT_DIR}"
        )
    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "\n======> Command failed with exit code ${status_code}! <======\n\n")
    endif()
endif()

list(APPEND DEPENDENCIES "${SCRIPT_DIR}/dependencies.cmake")
include("${buildtool_cmake}")
