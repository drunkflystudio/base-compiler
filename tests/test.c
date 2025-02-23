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

static const char* getFileName(const SourceFile* file)
{
    UNUSED(file);
    return "file";
}

static int getLineNumber(const SourceLine* line)
{
    return (line ? line->number : 0);
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
    int lexerState;

    Compiler* compiler = compilerPushNew(L);
    compilerSetLocationCallbacks(compiler, getFileName, getLineNumber);
    pushTestName(L);
    beginPrint(L);

    lua_checkstack(L, 10000);

    file = (SourceFile*)compilerTempAlloc(compiler, sizeof(SourceFile));
    file->name = g_testName;

    lexerState = LEXER_NORMAL;

    for (;;) {
        const char* lineStart;
        size_t lineLength;
        bool eof = !compilerReadLine(&fileContents, &lineStart, &lineLength);

        SourceLine* line = (SourceLine*)compilerTempAlloc(compiler, sizeof(SourceLine));
        line->number = lineNumber++;

        compilerBeginLine(compiler, file, line, lineStart, lineLength, &lexerState);

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
                printF(" (%s)", compilerPushHexString(L, compiler->lexer.token.integer));
                lua_pop(L, 1);
            }
            if (compiler->lexer.token.overflow)
                printF(" <overflow>");
            printC('\n');
        }

        if (eof) {
            if (lexerState == LEXER_MULTILINE_COMMENT)
                printF("(%d,%d): unterminated comment\n", line->number, compilerGetColumn(compiler));
            printF("(%d,%d): <end of file>\n", line->number, compilerGetColumn(compiler));
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

STRUCT(TestMethodArg)
{
    TestMethodArg* next;
    const char* name;
    CompilerExpr* value;
};

STRUCT(ParserTestContext)
{
    Compiler* compiler;
    lua_State* L;
    CompilerExpr* methodCallee;
    const char* methodName;
    TestMethodArg* methodFirstArg;
    TestMethodArg* methodLastArg;
};

static ParserTestMode g_parseMode;

#include "parsercb.h"

static int mystrcmp(const char* input, const char* pattern)
{
    for (;;) {
        if (*pattern != *input && *pattern != '?')
            return -1;
        if (*pattern == 0 || *input == 0)
            return 0;
        ++input;
        ++pattern;
    }
}

int test_parser(lua_State* L, ParserTestMode mode)
{
    const char* fileContents = luaL_checkstring(L, 1);
    const char* expected = replaceCRLF(L, luaL_checkstring(L, 2));
    const char* actual;
    ParserTestContext context;
    SourceFile* file;
    int lineNumber = 1;
    int lexerState;

    Compiler* compiler = compilerPushNew(L);
    compilerSetLocationCallbacks(compiler, getFileName, getLineNumber);
    pushTestName(L);
    beginPrint(L);

    lua_checkstack(L, 10000);

    file = (SourceFile*)compilerTempAlloc(compiler, sizeof(SourceFile));
    file->name = g_testName;

    context.compiler = compiler;
    context.L = L;

    compiler->parser.cb.ud = &context;
    initParserCallbacks(&compiler->parser);
    compilerBeginParse(compiler);

    g_indent = 0;
    g_parseMode = mode;

    switch (g_parseMode) {
        case PARSE_GLOBAL:
        case PARSE_ATTR:
            break;
        case PARSE_TYPES:
            lua_pushliteral(L, "class A{public var (\n");
            lua_pushvalue(L, 1);
            lua_pushliteral(L, "\n) x;}");
            lua_concat(L, 3);
            fileContents = lua_tostring(L, -1);
            break;
        case PARSE_EXPR:
            lua_pushliteral(L, "class A{public var x=\n");
            lua_pushvalue(L, 1);
            lua_pushliteral(L, "\n;}");
            lua_concat(L, 3);
            fileContents = lua_tostring(L, -1);
            break;
        case PARSE_STMT:
            lua_pushliteral(L, "class A{public(void)m{\n");
            lua_pushvalue(L, 1);
            lua_pushliteral(L, "\n}}");
            lua_concat(L, 3);
            fileContents = lua_tostring(L, -1);
            break;
    }

    lexerState = LEXER_NORMAL;

    for (;;) {
        const char* lineStart;
        size_t lineLength;
        bool eof = !compilerReadLine(&fileContents, &lineStart, &lineLength);

        SourceLine* line = (SourceLine*)compilerTempAlloc(compiler, sizeof(SourceLine));
        line->number = lineNumber++;

        compilerBeginLine(compiler, file, line, lineStart, lineLength, &lexerState);

        if (compiler->lexer.token.location.file != file) {
            logPrintf("%s: TEST FAILURE: invalid file in token location.\n", g_testName);
            ++g_testFailCount;
        }

        while (compilerGetToken(compiler))
            compilerPushToken(compiler);

        if (eof) {
            if (lexerState == LEXER_MULTILINE_COMMENT)
                printF("error: %d,%d: unterminated comment\n", line->number, compilerGetColumn(compiler));
            compilerPushTokenEof(compiler, line, compilerGetColumn(compiler));
            break;
        }
    }

    compilerEndParse(compiler);
    actual = endPrint();

    if (g_parseMode == PARSE_STMT) {
        static const char prefix[] = "stmtCompoundBegin loc:(1,22-1,22)\n";
        static const char suffix[] = "stmtCompoundEnd loc:(?,1-?,1)\n";
        size_t len;
        if (!strncmp(actual, prefix, sizeof(prefix) - 1))
            actual += sizeof(prefix) - 1;
        len = strlen(actual);
        if (len >= sizeof(suffix) - 1 && !mystrcmp(actual + len - sizeof(suffix) + 1, suffix))
            actual = lua_pushlstring(L, actual, len - sizeof(suffix) + 1);
    }

    if (!compare(L, expected, actual))
        ++g_testFailCount;
    else
        ++g_testSuccessCount;

    return 0;
}

/*===================================================================================================================*/
/* BOOTSTRAP CODEGEN */

int test_bootstrap(lua_State* L)
{
    const char* fileContents = luaL_checkstring(L, 1);
    const char* expected = replaceCRLF(L, luaL_checkstring(L, 2));
    const char* actual;
    CompilerOutputFile* output;
    SourceFile* file;
    int lineNumber = 1;
    int lexerState;

    Compiler* compiler = compilerPushNew(L);
    compilerSetLocationCallbacks(compiler, getFileName, getLineNumber);
    pushTestName(L);

    lua_checkstack(L, 10000);

    file = (SourceFile*)compilerTempAlloc(compiler, sizeof(SourceFile));
    file->name = g_testName;

    compilerInitBootstrapCodegen(compiler);
    compilerBeginParse(compiler);

    lexerState = LEXER_NORMAL;

    for (;;) {
        const char* lineStart;
        size_t lineLength;
        bool eof = !compilerReadLine(&fileContents, &lineStart, &lineLength);

        SourceLine* line = (SourceLine*)compilerTempAlloc(compiler, sizeof(SourceLine));
        line->number = lineNumber++;

        compilerBeginLine(compiler, file, line, lineStart, lineLength, &lexerState);

        if (compiler->lexer.token.location.file != file) {
            logPrintf("%s: TEST FAILURE: invalid file in token location.\n", g_testName);
            ++g_testFailCount;
        }

        while (compilerGetToken(compiler))
            compilerPushToken(compiler);

        if (eof) {
            if (lexerState == LEXER_MULTILINE_COMMENT)
                printF("error: %d,%d: unterminated comment\n", line->number, compilerGetColumn(compiler));
            compilerPushTokenEof(compiler, line, compilerGetColumn(compiler));
            break;
        }
    }

    compilerEndParse(compiler);

    output = compilerGetFirstOutput(compiler);
    if (!output) {
        logPrintf("%s: TEST FAILURE: no output files from the compiler.\n", g_testName);
        ++g_testFailCount;
        return 0;
    }
    if (compilerGetNextOutput(output)) {
        logPrintf("%s: TEST FAILURE: multiple output files from the compiler.\n", g_testName);
        ++g_testFailCount;
    }
    actual = compilerGetData(output, NULL);

    if (!compare(L, expected, actual))
        ++g_testFailCount;
    else
        ++g_testSuccessCount;

    return 0;
}
