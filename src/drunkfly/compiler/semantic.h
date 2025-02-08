#ifndef DRUNKFLY_COMPILER_SEMANTIC_H
#define DRUNKFLY_COMPILER_SEMANTIC_H

#include <drunkfly/compiler.h>

STRUCT(CompilerSemanticCallbacks)
{
    void* ud;
    void (*produceTextFile)(const char* name, const char* text);
};

 void compilerInitSemantic(Compiler* compiler);

#endif
