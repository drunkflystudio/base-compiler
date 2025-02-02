#ifndef DRUNKFLY_COMPILER_PRIVATE_H
#define DRUNKFLY_COMPILER_PRIVATE_H

#include <drunkfly/compiler/arena.h>
#include <drunkfly/compiler/lexer.h>

struct Compiler {
    CompilerArena* arenas;
    CompilerArena* largeBlocks;
    lua_State* L;
    CompilerLexer lexer;
};

#endif
