#ifndef DRUNKFLY_COMPILER_H
#define DRUNKFLY_COMPILER_H

#include <drunkfly/common.h>

STRUCT(CompilerToken);
STRUCT(CompilerLocation);
STRUCT(Compiler);

Compiler* compilerPushNew(lua_State* L);

#endif
