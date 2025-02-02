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
