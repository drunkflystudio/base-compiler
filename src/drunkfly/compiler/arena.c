#include <drunkfly/compiler/private.h>
#include <stdlib.h>
#include <string.h>

void compilerFreeArenas(Compiler* compiler, CompilerArena* arenas)
{
    CompilerArena* p, *prev;

    for (p = arenas; p; p = prev) {
        prev = p->prev;
        memFree(compiler->L, p, p->allocatedSize);
    }
}

void* compilerTempAlloc(Compiler* compiler, size_t size)
{
    CompilerArena* p;

    if (size >= COMPILER_ARENA_SIZE / 2) {
        size_t arenaSize = offsetof(CompilerArena, data) + size;
        p = (CompilerArena*)memAlloc(compiler->L, arenaSize);
        p->allocatedSize = arenaSize;
        p->bytesLeft = 0;
        p->prev = compiler->largeBlocks;
        compiler->largeBlocks = p;
        return p->data;
    }

    for (p = compiler->arenas; p != NULL; p = p->prev) {
        if (p->bytesLeft >= size)
            break;
    }

    if (!p) {
        p = (CompilerArena*)memAlloc(compiler->L, sizeof(CompilerArena));
        p->allocatedSize = sizeof(CompilerArena);
        p->bytesLeft = COMPILER_ARENA_SIZE;
        p->prev = compiler->arenas;
        compiler->arenas = p;
    }

    p->bytesLeft -= size;
    return &p->data[p->bytesLeft];
}

char* compilerTempDupStrN(Compiler* compiler, const char* str, size_t length)
{
    char* newStr = (char*)compilerTempAlloc(compiler, length + 1);
    memcpy(newStr, str, length);
    newStr[length] = 0;
    return newStr;
}

char* compilerTempDupStr(Compiler* compiler, const char* str)
{
    if (!str)
        return "";
    return compilerTempDupStrN(compiler, str, strlen(str));
}
