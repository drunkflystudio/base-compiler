#ifndef DRUNKFLY_COMPILER_ARENA_H
#define DRUNKFLY_COMPILER_ARENA_H

#include <drunkfly/compiler.h>

#define COMPILER_ARENA_SIZE 32768

STRUCT(CompilerArena) {
    CompilerArena* prev;
    size_t allocatedSize;
    size_t bytesLeft;
    char data[COMPILER_ARENA_SIZE];
};

void compilerFreeArenas(Compiler* compiler, CompilerArena* arenas);
void* compilerTempAlloc(Compiler* compiler, size_t size);
char* compilerTempDupStrN(Compiler* compiler, const char* str, size_t length);
char* compilerTempDupStr(Compiler* compiler, const char* str);

#endif
