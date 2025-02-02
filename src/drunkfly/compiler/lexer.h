#ifndef DRUNKFLY_COMPILER_LEXER_H
#define DRUNKFLY_COMPILER_LEXER_H

#include <drunkfly/compiler/token.h>

typedef enum LexerState {
    LEXER_NORMAL,
    LEXER_MULTILINE_COMMENT
} LexerState;

STRUCT(CompilerLexer)
{
    CompilerToken token;
    const char* start;
    const char* end;
    const char* cursor;
    const char* prevCursor;
    uint32_t curChar;
    int column;
    int state;
};

bool compilerReadLine(const char** ptr, const char** outLine, size_t* outLength);
void compilerBeginLine(Compiler* compiler, SourceFile* file, SourceLine* line, const char* text, size_t len, int state);
bool compilerGetToken(Compiler* compiler);

#endif
