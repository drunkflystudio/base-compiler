
get_filename_component(SCRIPT_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
list(APPEND DEPENDENCIES "${SCRIPT_DIR}/dependencies.cmake")
include("${SCRIPT_DIR}/../../ext/common/cmake/buildtool/buildtool.cmake")
