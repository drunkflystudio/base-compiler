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

void compilerInitLexer(Compiler* compiler, SourceFile* file, SourceLine* line, const char* text, int state);
bool compilerGetToken(Compiler* compiler);

#endif
