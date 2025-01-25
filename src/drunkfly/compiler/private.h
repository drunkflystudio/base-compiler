#ifndef DRUNKFLY_COMPILER_PRIVATE_H
#define DRUNKFLY_COMPILER_PRIVATE_H

#include <drunkfly/compiler/arena.h>

struct Compiler {
    lua_State* L;
    CompilerArena* arenas;
    CompilerArena* largeBlocks;
};

#endif
