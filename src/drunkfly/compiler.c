#include <drunkfly/compiler/private.h>
#include <drunkfly/compiler/arena.h>
#include <lua.h>
#include <lauxlib.h>
#include <string.h>

static const char TagCompilerMetatable = 0;

static int compilerFree(lua_State* L)
{
    Compiler* compiler = (Compiler*)lua_touserdata(L, 1);
    if (!compiler)
        return 0;

    compilerFreeArenas(compiler, compiler->arenas);
    compilerFreeArenas(compiler, compiler->largeBlocks);
    compiler->arenas = NULL;
    compiler->largeBlocks = NULL;

    return 0;
}

Compiler* compilerPushNew(lua_State* L)
{
    Compiler* compiler = (Compiler*)lua_newuserdata(L, sizeof(Compiler));
    if (!compiler)
        return NULL;

    memset(compiler, 0, sizeof(Compiler));

    if (luaL_newmetatable(L, &TagCompilerMetatable)) {
        lua_pushcfunction(L, compilerFree);
        lua_setfield(L, -2, "__gc");
    }
    lua_setmetatable(L, -2);

    compiler->arenas = NULL;
    compiler->largeBlocks = NULL;
    compiler->L = L;

    return compiler;
}

CompilerLocation* compilerMergeLocationsInto(Compiler* compiler,
    CompilerLocation* dst, const CompilerLocation* loc1, const CompilerLocation* loc2)
{
    if (!loc1 || !loc1->file) {
        if (loc2 && loc2->file) {
            *dst = *loc2;
            return dst;
        }

        dst->file = NULL;
        dst->startLine = NULL;
        dst->endLine = NULL;
        dst->startColumn = 0;
        dst->endColumn = 0;
        return dst;
    }

    if (!loc2 || !loc2->file) {
        *dst = *loc1;
        return dst;
    }

    if (loc1->file != loc2->file)
        luaL_error(compiler->L, "attempted to merge locations from different files.");

    dst->file = loc1->file;
    dst->startLine = loc1->startLine;
    dst->endLine = loc2->endLine;
    dst->startColumn = loc1->startColumn;
    dst->endColumn = loc2->endColumn;

    return dst;
}

CompilerLocation* compilerMergeLocations(Compiler* compiler, const CompilerLocation* lo1, const CompilerLocation* lo2)
{
    CompilerLocation* result = (CompilerLocation*)compilerTempAlloc(compiler, sizeof(CompilerLocation));
    return compilerMergeLocationsInto(compiler, result, lo1, lo2);
}
