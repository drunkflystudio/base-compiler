#ifndef DRUNKFLY_COMPILER_PRIVATE_H
#define DRUNKFLY_COMPILER_PRIVATE_H

#include <drunkfly/compiler/arena.h>
#include <drunkfly/compiler/lexer.h>
#include <drunkfly/compiler/parser.h>
#include <drunkfly/compiler/codegen.h>

struct Compiler {
    CompilerArena* arenas;
    CompilerArena* largeBlocks;
    lua_State* L;
    CompilerLexer lexer;
    CompilerParser parser;
    CompilerCodegen codegen;
    const char* (*getFileName)(const SourceFile* file);
    int (*getLineNumber)(const SourceLine* line);
};

#endif
