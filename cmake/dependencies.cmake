# This file is executed by buildtool before building the project
# Use it to setup the environment and compile additional tools if needed

require_gnu_tools()

build_host_tool(re2c
    DIRECTORY "${CURRENT_PROJECT_DIR}/tools/re2c"
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

build_host_tool(btyacc
    DIRECTORY "${CURRENT_PROJECT_DIR}/tools/btyacc"
    )
