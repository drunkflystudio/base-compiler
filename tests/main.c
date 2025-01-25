#include <drunkfly/compiler.h>
#include <drunkfly/vm.h>
#include <lua.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define RUN_FILE_NAME "t_compil.run"

static void printMessage(VMMSGTYPE type, const char* message)
{
    UNUSED(type);
    fprintf(stderr, "%s\n", message);
}

static int initTests(lua_State* L)
{
    UNUSED(L);
    return 0;
}

static int runTests(lua_State* L)
{
    UNUSED(L);
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
