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
    buffPrintF(&file->buffer, fmt, args);
    va_end(args);
}

const char* compilerGetData(CompilerOutputFile* file, size_t* size)
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
