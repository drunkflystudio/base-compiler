#ifndef DRUNKFLY_COMPILER_LEXER_H
#define DRUNKFLY_COMPILER_LEXER_H

#include <drunkfly/compiler/token.h>

STRUCT(CompilerLexer)
{
    CompilerToken token;
    const char* start;
    const char* cursor;
    int state;
};

void compilerInitLexer(CompilerLexer* lexer, SourceFile* file, SourceLine* line, const char* text, int state);
bool compilerGetToken(Compiler* compiler, lua_State* L, CompilerLexer* lexer);

#endif
