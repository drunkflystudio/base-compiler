#include "tests.h"

static const char* g_testName;

static void pushTestName(lua_State* L)
{
    lua_Debug ar;
    int line;
    lua_getstack(L, 1, &ar);
    lua_getinfo(L, "nSl", &ar);
    line = ar.currentline;
    g_testName = lua_pushfstring(L, "%s(%d)", g_testFileName, line);
}

int test_lexer(lua_State* L)
{
    const char* fileContents = luaL_checkstring(L, 1);
    const char* expected = replaceCRLF(L, luaL_checkstring(L, 2));

    Compiler* compiler = compilerPushNew(L);
    pushTestName(L);

    UNUSED(compiler);
    UNUSED(fileContents);
    UNUSED(expected);

    return 0;
}
