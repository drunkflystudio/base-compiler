#ifndef DRUNKFLY_COMPILER_CODEGEN_H
#define DRUNKFLY_COMPILER_CODEGEN_H

#include <drunkfly/compiler.h>

STRUCT(CompilerCodegen)
{
    Compiler* compiler;
    CompilerOutputFile* files;
};

void compilerInitBootstrapCodegen(Compiler* compiler, const char* fileName);
void compilerFinishBootstrapCodegen(Compiler* compiler);

CompilerOutputFile* compilerBeginOutput(Compiler* compiler, const char* fileName);
void compilerPrintC(CompilerOutputFile* file, uint32_t c);
void compilerPrintS(CompilerOutputFile* file, const char* str);
void compilerPrintF(CompilerOutputFile* file, const char* fmt, ...);

#endif
