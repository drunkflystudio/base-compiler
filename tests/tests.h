#ifndef COMPILER_TESTS_COMMON_H
#define COMPILER_TESTS_COMMON_H

#include <drunkfly/compiler.h>
#include <drunkfly/compiler/private.h>
#include <drunkfly/compiler/parser.h>
#include <drunkfly/vm.h>
#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

typedef enum ParserTestMode {
    PARSE_GLOBAL,
    PARSE_ATTR,
    PARSE_EXPR,
    PARSE_TYPES
} ParserTestMode;

extern const char* g_testFileName;
extern int g_testFailCount;
extern int g_testSuccessCount;

/* test.c */

int test_lexer(lua_State* L);
int test_parser(lua_State* L, ParserTestMode mode);

/* util.c */

void beginPrint(lua_State* L);
void printC(char ch);
void printS(const char* str);
void printF(const char* fmt, ...);
const char* endPrint(void);
const char* boolStr(bool value);
const char* replaceCRLF(lua_State* L, const char* src);
void logPrintf(const char* fmt, ...);
bool writeFile(const char* name, const char* contents);

#endif
