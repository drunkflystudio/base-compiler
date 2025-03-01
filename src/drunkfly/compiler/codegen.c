#include <drunkfly/compiler/codegen.h>
#include <drunkfly/compiler/private.h>
#include <drunkfly/buff.h>

struct CompilerOutputFile
{
    CompilerOutputFile* next;
    const char* name;
    Buff buffer;
};

CompilerOutputFile* compilerBeginOutput(Compiler* compiler, const char* fileName)
{
    CompilerOutputFile* file = (CompilerOutputFile*)compilerTempAlloc(compiler, sizeof(CompilerOutputFile));
    file->name = (fileName ? compilerTempDupStr(compiler, fileName) : NULL);

    if (!fileName) {
        CompilerOutputFile* p;
        for (p = compiler->codegen.files; p; p = p->next) {
            if (p->name == NULL)
                luaL_error(compiler->L, "more than one unnamed output file.");
        }
    }

    buffInit(&file->buffer, compiler->L);

    file->next = compiler->codegen.files;
    compiler->codegen.files = file;

    return file;
}

void compilerPrintC(CompilerOutputFile* file, uint32_t c)
{
    buffPrintUtf8(&file->buffer, c);
}

void compilerPrintS(CompilerOutputFile* file, const char* str)
{
    buffPrintS(&file->buffer, str);
}

void compilerPrintF(CompilerOutputFile* file, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    buffPrintV(&file->buffer, fmt, args);
    va_end(args);
}

const char* compilerGetOutputName(CompilerOutputFile* file)
{
    return file->name;
}

const char* compilerGetOutputData(CompilerOutputFile* file, size_t* size)
{
    return buffEnd(&file->buffer, size);
}

CompilerOutputFile* compilerGetFirstOutput(Compiler* compiler)
{
    return compiler->codegen.files;
}

CompilerOutputFile* compilerGetNextOutput(CompilerOutputFile* file)
{
    return file->next;
}

const char* compilerPushHexString(lua_State* L, uint_value_t value)
{
    char buf[64];
    char* dst = buf + sizeof(buf);

    *--dst = 0;

    do {
        *--dst = luastr_hex[1 + (value & 0xf)];
        value >>= 4;
    } while (value);

    *--dst = 'x';
    *--dst = '0';

    return lua_pushstring(L, dst);
}

const char* compilerPushUtf8String(lua_State* L, const uint32_t* text, size_t textLength, bool escape)
{
    size_t i;

    luaL_Buffer buffer;
    luaL_buffinit(L, &buffer);

    for (i = 0; i < textLength; i++) {
        uint32_t ch = *text++;
        if (escape) {
            switch (ch) {
                case 0:    luaL_addlstring(&buffer, "\\x00", 4); continue;
                case 1:    luaL_addlstring(&buffer, "\\x01", 4); continue;
                case 2:    luaL_addlstring(&buffer, "\\x02", 4); continue;
                case 3:    luaL_addlstring(&buffer, "\\x03", 4); continue;
                case 4:    luaL_addlstring(&buffer, "\\x04", 4); continue;
                case 5:    luaL_addlstring(&buffer, "\\x05", 4); continue;
                case 6:    luaL_addlstring(&buffer, "\\x06", 4); continue;
                case 7:    luaL_addlstring(&buffer, "\\a", 2); continue;
                case 8:    luaL_addlstring(&buffer, "\\b", 2); continue;
                case 9:    luaL_addlstring(&buffer, "\\x09", 4); continue;
                case 10:   luaL_addlstring(&buffer, "\\n", 2); continue;
                case 11:   luaL_addlstring(&buffer, "\\v", 2); continue;
                case 12:   luaL_addlstring(&buffer, "\\f", 2); continue;
                case 13:   luaL_addlstring(&buffer, "\\r", 2); continue;
                case 14:   luaL_addlstring(&buffer, "\\x0e", 4); continue;
                case 15:   luaL_addlstring(&buffer, "\\x0f", 4); continue;
                case 16:   luaL_addlstring(&buffer, "\\x10", 4); continue;
                case 17:   luaL_addlstring(&buffer, "\\x11", 4); continue;
                case 18:   luaL_addlstring(&buffer, "\\x12", 4); continue;
                case 19:   luaL_addlstring(&buffer, "\\x13", 4); continue;
                case 20:   luaL_addlstring(&buffer, "\\x14", 4); continue;
                case 21:   luaL_addlstring(&buffer, "\\x15", 4); continue;
                case 22:   luaL_addlstring(&buffer, "\\x16", 4); continue;
                case 23:   luaL_addlstring(&buffer, "\\x17", 4); continue;
                case 24:   luaL_addlstring(&buffer, "\\x18", 4); continue;
                case 25:   luaL_addlstring(&buffer, "\\x19", 4); continue;
                case 26:   luaL_addlstring(&buffer, "\\x1a", 4); continue;
                case 27:   luaL_addlstring(&buffer, "\\x1b", 4); continue;
                case 28:   luaL_addlstring(&buffer, "\\x1c", 4); continue;
                case 29:   luaL_addlstring(&buffer, "\\x1d", 4); continue;
                case 30:   luaL_addlstring(&buffer, "\\x1e", 4); continue;
                case 31:   luaL_addlstring(&buffer, "\\x1f", 4); continue;
                case '\\': luaL_addlstring(&buffer, "\\\\", 2); continue;
                case '"':  luaL_addlstring(&buffer, "\\\"", 2); continue;
                case 127:  luaL_addlstring(&buffer, "\\x7f", 4); continue;
            }
        }
        /* FIXME: UTF-8 */
        luaL_addchar(&buffer, (char)ch);
    }

    return luaL_pushresult(&buffer);
}
