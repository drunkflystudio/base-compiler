#ifndef DRUNKFLY_COMPILER_BUNDLE_H
#define DRUNKFLY_COMPILER_BUNDLE_H

#include <drunkfly/compiler.h>

CompilerBundle* compilerBeginBundle(Compiler* compiler, const char* outputFile, const char* bundleName);
void compilerEndBundle(Compiler* compiler);

void compilerBundleBeginClass(CompilerBundle* bundle, const char* name);
void compilerBundleBeginInterface(CompilerBundle* bundle, const char* name);
void compilerBundleParentClass(CompilerBundle* bundle, const char* name);
void compilerBundleFinalClass(CompilerBundle* bundle);
void compilerBundleBeginClassMembers(CompilerBundle* bundle);
void compilerBundleBeginClassMethod(CompilerBundle* bundle, CompilerVisibility vis);
void compilerBundleBeginInterfaceMethod(CompilerBundle* bundle);
void compilerBundleStaticMethod(CompilerBundle* bundle);
void compilerBundleBeginMethodReturnType(CompilerBundle* bundle);
void compilerBundleEndMethodReturnType(CompilerBundle* bundle);
void compilerBundleSimpleMethod(CompilerBundle* bundle, const char* name);
void compilerBundleMethodArg(CompilerBundle* bundle, const char* name);
void compilerBundleMethodArgName(CompilerBundle* bundle, const char* name);
void compilerBundleFinalMethod(CompilerBundle* bundle);
void compilerBundleOverrideMethod(CompilerBundle* bundle);
void compilerBundleEndMethod(CompilerBundle* bundle);
void compilerBundleEndMethod_Abstract(CompilerBundle* bundle);
void compilerBundleEndMethod_Extern(CompilerBundle* bundle);
void compilerBundleEndClassOrInterface(CompilerBundle* bundle);

void compilerBundleType(CompilerBundle* bundle, const char* type);
void compilerBundleTypePointer(CompilerBundle* bundle);

#endif
