#include "tests.h"
#include "conf.h"

static const char* g_testName;

struct SourceFile
{
    const char* name;
};

struct SourceLine
{
    int number;
};

static void pushTestName(lua_State* L)
{
    lua_Debug ar;
    int line;
    lua_getstack(L, 1, &ar);
    lua_getinfo(L, "nSl", &ar);
    line = ar.currentline;
    g_testName = lua_pushfstring(L, "%s(%d)", g_testFileName, line);
}

static bool compare(lua_State* L, const char* expected, const char* actual)
{
    if (strcmp(expected, actual) != 0) {
        logPrintf("%s: TEST FAILURE!\n"
            "###########################\n",
            g_testName);
        if (writeFile("expected", expected) && writeFile("actual", actual)) {
          #ifdef DIFF_EXE
            const char* diff = DIFF_EXE;
          #else
            const char* diff = "diff";
          #endif
            int r = system(lua_pushfstring(L, "%s -u expected actual", diff));
            (void)r; /* silence compiler warning of unused system() return value */
        }
        logPrintf(
            "################## EXPECTED\n%s"
            "################## ACTUAL\n%s",
            expected, actual);
        return false;
    }
    return true;
}

static const char* pushHex(lua_State* L, uint_value_t value)
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

/*===================================================================================================================*/
/* LEXER */

int test_lexer(lua_State* L)
{
    const char* fileContents = luaL_checkstring(L, 1);
    const char* expected = replaceCRLF(L, luaL_checkstring(L, 2));
    const char* actual;
    SourceFile* file;
    int lineNumber = 1;

    Compiler* compiler = compilerPushNew(L);
    pushTestName(L);
    beginPrint(L);

    file = (SourceFile*)compilerTempAlloc(compiler, sizeof(SourceFile));
    file->name = g_testName;

    compiler->lexer.state = LEXER_NORMAL;

    for (;;) {
        const char* lineStart;
        size_t lineLength;
        bool eof = !compilerReadLine(&fileContents, &lineStart, &lineLength);

        SourceLine* line = (SourceLine*)compilerTempAlloc(compiler, sizeof(SourceLine));
        line->number = lineNumber++;

        compilerBeginLine(compiler, file, line, lineStart, lineLength, compiler->lexer.state);

        if (compiler->lexer.token.location.file != file) {
            logPrintf("%s: TEST FAILURE: invalid file in token location.\n", g_testName);
            ++g_testFailCount;
        }

        while (compilerGetToken(compiler)) {
            if (!compiler->lexer.token.location.startLine) {
                logPrintf("%s: TEST FAILURE: %s is NULL in token location.\n", g_testName, "startLine");
                ++g_testFailCount;
            }
            if (!compiler->lexer.token.location.endLine) {
                logPrintf("%s: TEST FAILURE: %s is NULL in token location.\n", g_testName, "endLine");
                ++g_testFailCount;
            }
            printF("(%d,%d-%d,%d): %s",
                (compiler->lexer.token.location.startLine ? compiler->lexer.token.location.startLine->number : 0),
                compiler->lexer.token.location.startColumn,
                (compiler->lexer.token.location.endLine ? compiler->lexer.token.location.endLine->number : 0),
                compiler->lexer.token.location.endColumn,
                compiler->lexer.token.name);
            if (compiler->lexer.token.text)
                printF(" \"%s\"", compiler->lexer.token.text);
            if (compiler->lexer.token.id == T_INTEGER_LITERAL) {
                printF(" (%s)", pushHex(L, compiler->lexer.token.integer));
                lua_pop(L, 1);
            }
            if (compiler->lexer.token.overflow)
                printF(" <overflow>");
            printC('\n');
        }

        if (eof) {
            if (compiler->lexer.state == LEXER_MULTILINE_COMMENT)
                printF("(%d,%d): unterminated comment\n", line->number, compiler->lexer.column);
            printF("(%d,%d): <end of file>\n", line->number, compiler->lexer.column);
            break;
        }
    }

    actual = endPrint();

    if (!compare(L, expected, actual))
        ++g_testFailCount;
    else
        ++g_testSuccessCount;

    return 0;
}

/*===================================================================================================================*/
/* PARSER */

STRUCT(ParserTestContext)
{
    Compiler* compiler;
    lua_State* L;
};

#include "parsercb.h"

int test_parser_full(lua_State* L)
{
    const char* fileContents = luaL_checkstring(L, 1);
    const char* expected = replaceCRLF(L, luaL_checkstring(L, 2));
    const char* actual;
    CompilerParser parser;
    ParserTestContext context;
    SourceFile* file;
    int lineNumber = 1;

    Compiler* compiler = compilerPushNew(L);
    pushTestName(L);
    beginPrint(L);

    file = (SourceFile*)compilerTempAlloc(compiler, sizeof(SourceFile));
    file->name = g_testName;

    context.compiler = compiler;
    context.L = L;

    memset(&parser, 0, sizeof(CompilerParser));
    parser.compiler = compiler;
    parser.cb.ud = &context;
    initParserCallbacks(&parser);
    compilerBeginParse(&parser);

    g_indent = 0;
    compiler->lexer.state = LEXER_NORMAL;

    for (;;) {
        const char* lineStart;
        size_t lineLength;
        bool eof = !compilerReadLine(&fileContents, &lineStart, &lineLength);

        SourceLine* line = (SourceLine*)compilerTempAlloc(compiler, sizeof(SourceLine));
        line->number = lineNumber++;

        compilerBeginLine(compiler, file, line, lineStart, lineLength, compiler->lexer.state);

        if (compiler->lexer.token.location.file != file) {
            logPrintf("%s: TEST FAILURE: invalid file in token location.\n", g_testName);
            ++g_testFailCount;
        }

        while (compilerGetToken(compiler))
            compilerPushToken(&parser);

        if (eof) {
            if (compiler->lexer.state == LEXER_MULTILINE_COMMENT)
                printF("error: %d,%d: unterminated comment\n", line->number, compiler->lexer.column);
            compiler->lexer.token.id = T_EOF;
            compiler->lexer.token.name = "<end of file>";
            compiler->lexer.token.location.startLine = line;
            compiler->lexer.token.location.endLine = line;
            compiler->lexer.token.location.startColumn = compiler->lexer.column;
            compiler->lexer.token.location.endColumn = compiler->lexer.column;
            compilerPushToken(&parser);
            break;
        }
    }

    compilerEndParse(&parser);
    actual = endPrint();

    if (!compare(L, expected, actual))
        ++g_testFailCount;
    else
        ++g_testSuccessCount;

    return 0;
}
