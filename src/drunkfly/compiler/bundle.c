#include <drunkfly/compiler/bundle.h>
#include <drunkfly/compiler/arena.h>
#include <drunkfly/compiler/codegen.h>
#include <drunkfly/compiler/private.h>
#include <drunkfly/buff.h>
#include <string.h>

struct CompilerBundle {
    Compiler* compiler;
    const char* name;
    CompilerOutputFile* file;
};

CompilerBundle* compilerBeginBundle(Compiler* compiler, const char* outputFile, const char* bundleName)
{
    CompilerBundle* bundle = (CompilerBundle*)compilerTempAlloc(compiler, sizeof(CompilerBundle));
    memset(bundle, 0, sizeof(CompilerBundle));
    bundle->compiler = compiler;
    bundle->file = compilerBeginOutput(compiler, outputFile);
    bundle->name = compilerTempDupStr(compiler, bundleName);
    compiler->bundle = bundle;
    return bundle;
}

void compilerEndBundle(Compiler* compiler)
{
    compiler->bundle = NULL;
}

void compilerBundleAddClass(CompilerBundle* bundle, const char* name)
{
    compilerPrintF(bundle->file, "class %s\n", name);
}
