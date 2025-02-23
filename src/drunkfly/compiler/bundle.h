#ifndef DRUNKFLY_COMPILER_BUNDLE_H
#define DRUNKFLY_COMPILER_BUNDLE_H

#include <drunkfly/compiler.h>

CompilerBundle* compilerBeginBundle(Compiler* compiler, const char* outputFile, const char* bundleName);
void compilerEndBundle(Compiler* compiler);

void compilerBundleAddClass(CompilerBundle* bundle, const char* name);

#endif
