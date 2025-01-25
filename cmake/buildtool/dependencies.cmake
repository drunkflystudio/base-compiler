
require_ninja()
if(WIN32)
    require_mingw810_32()
endif()

######################################################################################################################
# Re2C

get_build_types(build_types "${RE2C}")
foreach(build_type ${build_types})
    set(dir "_host_/re2c/${build_type}")
    generate_project(
        EXECUTABLE "re2c.exe"
        DIRECTORY "${dir}"
        PROJECT_DIR "${PROJECT_DIR}/tools/re2c"
        BUILD_TYPE "${build_type}"
        REGISTER_AS HOST_RE2C
        OPTIONS
            RE2C_REBUILD_DOCS=FALSE
            RE2C_BUILD_BENCHMARKS=FALSE
            RE2C_BUILD_TESTS=FALSE
            RE2C_BUILD_RE2D=FALSE
            RE2C_BUILD_RE2GO=FALSE
            RE2C_BUILD_RE2HS=FALSE
            RE2C_BUILD_RE2JAVA=FALSE
            RE2C_BUILD_RE2JS=FALSE
            RE2C_BUILD_RE2OCAML=FALSE
            RE2C_BUILD_RE2PY=FALSE
            RE2C_BUILD_RE2RUST=FALSE
            RE2C_BUILD_RE2V=FALSE
            RE2C_BUILD_RE2ZIG=FALSE
        )
    build_project(
        DIRECTORY "${dir}"
        )
endforeach()

######################################################################################################################
# BtYacc

get_build_types(build_types "${RE2C}")
foreach(build_type ${build_types})
    set(dir "_host_/btyacc/${build_type}")
    generate_project(
        EXECUTABLE "btyacc.exe"
        DIRECTORY "${dir}"
        PROJECT_DIR "${PROJECT_DIR}/tools/btyacc"
        BUILD_TYPE "${build_type}"
        REGISTER_AS HOST_BTYACC
        )
    build_project(
        DIRECTORY "${dir}"
        )
endforeach()
