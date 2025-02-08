#include "tests.h"
#include <signal.h>

#define RUN_FILE_NAME "t_compil.run"

lua_State* g_L;
const char* g_testFileName;
int g_testFailCount;
int g_testSuccessCount;

static void printMessage(VMMSGTYPE type, const char* message)
{
    UNUSED(type);
    fprintf(stderr, "%s\n", message);
}

int test_parser_global(lua_State* L) { return test_parser(L, PARSE_GLOBAL); }
int test_parser_attr(lua_State* L) { return test_parser(L, PARSE_ATTR); }
int test_parser_expr(lua_State* L) { return test_parser(L, PARSE_EXPR); }
int test_parser_types(lua_State* L) { return test_parser(L, PARSE_TYPES); }

static int initTests(lua_State* L)
{
    g_L = L;

    lua_pushcfunction(L, test_lexer);
    lua_setglobal(L, "test_lexer");

    lua_pushcfunction(L, test_parser_global);
    lua_setglobal(L, "test_parser_global");
    lua_pushcfunction(L, test_parser_attr);
    lua_setglobal(L, "test_parser_attr");
    lua_pushcfunction(L, test_parser_expr);
    lua_setglobal(L, "test_parser_expr");
    lua_pushcfunction(L, test_parser_types);
    lua_setglobal(L, "test_parser_types");

    return 0;
}

static void signalHandler(int i)
{
    signal(i, SIG_DFL);
    vmInterrupt(g_L);
}

static int runTest(lua_State* L, const char* name, const char* test, size_t testLen)
{
    int status;

    g_testFileName = name + 1;

    status = luaL_loadbuffer(L, test, testLen, name);
    if (status == LUA_OK) {
        signal(SIGINT, signalHandler);
        status = vmProtectedCall(L, 0, 0);
        signal(SIGINT, SIG_DFL);
    }

    if (status != LUA_OK)
        g_testFailCount++;

    return vmCheckError(L, status);
}

static int runTests(lua_State* L)
{
    #include "testlist.h"

    if (g_testFailCount == 0) {
        logPrintf("=== %d COMPILER TEST%s SUCCEEDED ===\n", g_testSuccessCount, (g_testSuccessCount == 1 ? "" : "S"));
        lua_pushboolean(L, 1);
    } else {
        logPrintf("=== %d COMPILER TEST%s FAILED ===\n", g_testFailCount, (g_testFailCount == 1 ? "" : "S"));
        lua_pushboolean(L, 0);
    }

    return 0;
}

int main(void)
{
    bool success;

    remove(RUN_FILE_NAME);

    p_luaX_init = luaX_init;
    p_luaY_parser = luaY_parser;
    success = vmRun(initTests, runTests, printMessage, NULL);

    if (success) {
        FILE* f = fopen(RUN_FILE_NAME, "w");
        if (f)
            fclose(f);
        else {
            fprintf(stderr, "unable to open file \"%s\" for writing: %s\n", RUN_FILE_NAME, strerror(errno));
            success = false;
        }
    }

    return (success ? EXIT_SUCCESS : EXIT_FAILURE);
}
