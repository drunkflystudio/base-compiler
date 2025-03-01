#include "tests.h"
#include <drunkfly/buff.h>

static Buff g_buffer;
FILE* g_logFile;

void beginPrint(lua_State* L)
{
    buffInit(&g_buffer, L);
}

void printC(char ch)
{
    buffPrintC(&g_buffer, ch);
}

void printUtf8(uint32_t ch)
{
    buffPrintUtf8(&g_buffer, ch);
}

void printS(const char* str)
{
    buffPrintS(&g_buffer, str);
}

void printF(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    buffPrintV(&g_buffer, fmt, args);
    va_end(args);
}

const char* endPrint(void)
{
    return buffEnd(&g_buffer, NULL);
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
        int err = errno;
        fclose(f);
        fprintf(stderr, "error: can't write file \"%s\": %s", name, strerror(err));
        return false;
    }
    fclose(f);
    return true;
}
