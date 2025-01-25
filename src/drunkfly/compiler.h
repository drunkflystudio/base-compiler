#ifndef DRUNKFLY_COMPILER_H
#define DRUNKFLY_COMPILER_H

#include <drunkfly/common.h>

STRUCT(Location);
STRUCT(Token);
STRUCT(Compiler);

Compiler* compilerPushNew(lua_State* L);

#endif
