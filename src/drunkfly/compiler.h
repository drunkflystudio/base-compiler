#ifndef DRUNKFLY_COMPILER_H
#define DRUNKFLY_COMPILER_H

#include <drunkfly/common.h>

STRUCT(SourceFile);
STRUCT(SourceLine);

STRUCT(CompilerToken);
STRUCT(CompilerLocation);
STRUCT(Compiler);

STRUCT(CompilerType);
STRUCT(CompilerExpr);

typedef enum CompilerVisibility {
    COMPILER_PRIVATE,
    COMPILER_PROTECTED,
    COMPILER_PUBLIC
} CompilerVisibility;

Compiler* compilerPushNew(lua_State* L);

CompilerLocation* compilerMergeLocationsInto(Compiler* compiler,
    CompilerLocation* dst, const CompilerLocation* loc1, const CompilerLocation* loc2);
CompilerLocation* compilerMergeLocations(Compiler* compiler, const CompilerLocation* lo1, const CompilerLocation* lo2);

#endif
