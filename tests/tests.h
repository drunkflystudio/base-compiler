#ifndef COMPILER_TESTS_COMMON_H
#define COMPILER_TESTS_COMMON_H

#include <drunkfly/compiler.h>
#include <drunkfly/vm.h>
#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void beginPrint(lua_State* L);
void printC(char ch);
void printS(const char* str);
void printF(const char* fmt, ...);
const char* endPrint(void);

const char* boolStr(bool value);

const char* replaceCRLF(lua_State* L, const char* src);

void logPrintf(const char* fmt, ...);

#endif
