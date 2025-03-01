#include <drunkfly/compiler.h>
#include <drunkfly/compiler/codegen.h>
#include <drunkfly/compiler/lexer.h>
#include <drunkfly/compiler/parser.h>
#include <drunkfly/compiler/arena.h>
#include <drunkfly/compiler/bundle.h>
#include <drunkfly/vm.h>
#include <lua.h>
#include <lauxlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Codegen {
    CODEGEN_NONE = 0,
    CODEGEN_BOOTSTRAP
} Codegen;

struct SourceFile
{
    SourceFile* next;
    const char* path;
};

struct SourceLine
{
    int number;
};

static Compiler* g_compiler;
static int g_argc;
static char** g_argv;
static int g_exitCode;

static const char* getFileName(const SourceFile* file)
{
    return (file ? file->path : NULL);
}

static int getLineNumber(const SourceLine* line)
{
    return (line ? line->number : 0);
}

/*
static const char* formatLocation(lua_State* L, const CompilerLocation* loc)
{
    if (!loc)
        return "";

    return lua_pushfstring(L, "%s(%d,%d-%d,%d)",
        getFileName(loc->file),
        getLineNumber(loc->startLine),
        loc->startColumn,
        getLineNumber(loc->endLine),
        loc->endColumn);
}
*/

static void printMessage(VMMSGTYPE type, const char* message)
{
    UNUSED(type);
    fprintf(stderr, "%s\n", message);
}

static char* loadFile(lua_State* L, const char* path, size_t* fileSize)
{
    size_t size, bytesRead;
    char* buffer;

    FILE* f = fopen(path, "rb");
    if (!f)
        luaL_error(L, "error: unable to open file \"%s\": %s", path, strerror(errno));

    fseek(f, 0, SEEK_END);
    size = (size_t)ftell(f);
    fseek(f, 0, SEEK_SET);
    if (ferror(f)) {
        int err = errno;
        fclose(f);
        luaL_error(L, "error: unable to determine size of file \"%s\": %s", path, strerror(err));
    }

    buffer = (char*)compilerTempAlloc(g_compiler, size + 1);
    buffer[size] = 0;

    bytesRead = fread(buffer, 1, size, f);
    if (ferror(f) || bytesRead != size) {
        int err = errno;
        fclose(f);
        luaL_error(L, "error: unable to read file \"%s\": %s", path, strerror(err));
    }

    fclose(f);

    if (fileSize)
        *fileSize = size;

    return buffer;
}

static void writeFile(lua_State* L, const char* path, const char* data, size_t size)
{
    size_t bytesWritten;
    FILE* f;

    f = fopen(path, "w");
    if (!f)
        luaL_error(L, "error: can't write file \"%s\": %s", path, strerror(errno));

    bytesWritten = fwrite(data, 1, size, f);
    if (ferror(f) || bytesWritten != size) {
        int err = errno;
        fclose(f);
        luaL_error(L, "error: can't write file \"%s\": %s", path, strerror(err));
    }

    fclose(f);
}

static void compileFile(lua_State* L, SourceFile* file)
{
    const char* fileContents;
    int lexerState = LEXER_NORMAL;
    int lineNumber = 1;

    fileContents = loadFile(L, file->path, NULL);

    compilerBeginParse(g_compiler);
    lexerState = LEXER_NORMAL;

    for (;;) {
        const char* lineStart;
        size_t lineLength;
        bool eof = !compilerReadLine(&fileContents, &lineStart, &lineLength);

        SourceLine* line = (SourceLine*)compilerTempAlloc(g_compiler, sizeof(SourceLine));
        line->number = lineNumber++;

        compilerBeginLine(g_compiler, file, line, lineStart, lineLength, &lexerState);
        while (compilerGetToken(g_compiler))
            compilerPushToken(g_compiler);

        if (eof) {
            if (lexerState == LEXER_MULTILINE_COMMENT) {
                luaL_error(L, "%s(%d,%d): unterminated comment\n",
                    file->path, line->number, compilerGetColumn(g_compiler));
            }
            compilerPushTokenEof(g_compiler, line, compilerGetColumn(g_compiler));
            break;
        }
    }

    compilerEndParse(g_compiler);
}

static int runCompiler(lua_State* L)
{
    const char* outputName = "output.c";
    const char* bundleName = NULL;
    SourceFile* firstSource = NULL;
    SourceFile* lastSource = NULL;
    Codegen codegen = CODEGEN_NONE;
    CompilerOutputFile* output;
    const char* outputData;
    size_t outputSize;
    int i;

    lua_checkstack(L, LUAI_MAXSTACK);

    g_compiler = compilerPushNew(L);
    compilerSetLocationCallbacks(g_compiler, getFileName, getLineNumber);

    for (i = 1; i < g_argc; i++) {
        if (g_argv[i][0] != '-') {
            SourceFile* file = (SourceFile*)compilerTempAlloc(g_compiler, sizeof(SourceFile));
            file->next = NULL;
            file->path = g_argv[i];
            if (!firstSource)
                firstSource = file;
            else
                lastSource->next = file;
            lastSource = file;
        } else if (!strcmp(g_argv[i], "-bootstrap")) {
            codegen = CODEGEN_BOOTSTRAP;
        } else if (!strcmp(g_argv[i], "-o")) {
            if (i == g_argc - 1)
                luaL_error(L, "missing output file name after '%s'.", g_argv[i]);
            outputName = g_argv[++i];
        } else if (!strcmp(g_argv[i], "-bundle")) {
            if (i == g_argc - 1)
                luaL_error(L, "missing bundle name after '%s'.", g_argv[i]);
            bundleName = g_argv[++i];
        } else if (!strcmp(g_argv[i], "-h") || !strcmp(g_argv[i], "-help") || !strcmp(g_argv[i], "--help")) {
            printf("Usage: flycc [options] file...\n"
                   "where options are:\n"
                   "  -o <path>            specify name of the output file.\n"
                );
            return g_exitCode = EXIT_FAILURE, 0;
        } else {
            fprintf(stderr, "unknown command line argument: '%s'.\n", g_argv[i]);
            return g_exitCode = EXIT_FAILURE, 0;
        }
    }

    if (!firstSource) {
        fprintf(stderr, "no input files specified.\n");
        return g_exitCode = EXIT_FAILURE, 0;
    }

    switch (codegen) {
        case CODEGEN_NONE:
            /* FIXME */
            break;
        case CODEGEN_BOOTSTRAP:
            compilerInitBootstrapCodegen(g_compiler, outputName);
            break;
    }

    if (bundleName) {
        const char* bundleFile;
        char* slash = strrchr(outputName, '/');
      #ifdef _WIN32
        char* slash2 = strrchr(outputName, '\\');
        if (slash2 && (!slash || slash2 > slash))
            slash = slash2;
      #endif
        if (slash)
            lua_pushlstring(L, outputName, (size_t)(slash - outputName + 1));
        bundleFile = lua_pushfstring(L, "%s.bun", bundleName);
        if (slash) {
            lua_concat(L, 2);
            bundleFile = lua_tostring(L, -1);
        }
        compilerBeginBundle(g_compiler, bundleFile, bundleName);
    }

    while (firstSource) {
        printf("%s\n", firstSource->path);
        compileFile(L, firstSource);
        firstSource = firstSource->next;
    }

    compilerEndBundle(g_compiler);

    switch (codegen) {
        case CODEGEN_NONE:
            /* FIXME */
            break;
        case CODEGEN_BOOTSTRAP:
            compilerFinishBootstrapCodegen(g_compiler);
            break;
    }

    output = compilerGetFirstOutput(g_compiler);
    while (output) {
        const char* name = compilerGetOutputName(output);
        outputData = compilerGetOutputData(output, &outputSize);
        writeFile(L, name, outputData, outputSize);
        output = compilerGetNextOutput(output);
    }

    return 0;
}

int main(int argc, char** argv)
{
    bool success;

    g_argc = argc;
    g_argv = argv;
    g_exitCode = EXIT_SUCCESS;

    success = vmRun(NULL, runCompiler, printMessage, NULL);
    return (success ? g_exitCode : EXIT_FAILURE);
}
