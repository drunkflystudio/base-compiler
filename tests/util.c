#include "tests.h"
#include <lobject.h>
#include <stdarg.h>

static luaL_Buffer g_buffer;
FILE* g_logFile;

void beginPrint(lua_State* L)
{
    luaL_buffinit(L, &g_buffer);
}

void printC(char ch)
{
    luaL_addchar(&g_buffer, ch);
}

void printS(const char* str)
{
    luaL_addstring(&g_buffer, str);
}

void printF(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    luaO_pushvfstring(g_buffer.L, fmt, args);
    va_end(args);
    luaL_addvalue(&g_buffer);
}

const char* endPrint(void)
{
    luaL_pushresult(&g_buffer);
    return lua_tostring(g_buffer.L, -1);
}

const char* boolStr(bool value)
{
    return (value ? "true" : "false");
}

const char* replaceCRLF(lua_State* L, const char* src)
{
    luaL_Buffer buffer;
    size_t len = strlen(src);
    luaL_buffinitsize(L, &buffer, len);
    while (*src) {
        if (*src == '\r') {
            ++src;
            if (*src == '\n')
                ++src;
            luaL_addchar(&buffer, '\n');
        } else
            luaL_addchar(&buffer, *src++);
    }
    luaL_pushresult(&buffer);
    return lua_tostring(L, -1);
}

void logPrintf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);

    if (g_logFile)
        vfprintf(g_logFile, fmt, args);

    va_end(args);
}

bool writeFile(const char* name, const char* contents)
{
    FILE* f = fopen(name, "wb");
    if (!f) {
        fprintf(stderr, "error: can't write file \"%s\": %s", name, strerror(errno));
        return false;
    }
    fwrite(contents, 1, strlen(contents), f);
    if (ferror(f)) {
        fclose(f);
        fprintf(stderr, "error: can't write file \"%s\": %s", name, strerror(errno));
        return false;
    }
    fclose(f);
    return true;
}
