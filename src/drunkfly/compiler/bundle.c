#include <drunkfly/compiler/bundle.h>
#include <drunkfly/compiler/arena.h>
#include <drunkfly/compiler/codegen.h>
#include <drunkfly/compiler/private.h>
#include <drunkfly/buff.h>
#include <string.h>
#include <assert.h>

struct CompilerBundle {
    Compiler* compiler;
    const char* name;
    CompilerOutputFile* file;
    const char* prefix;
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

void compilerBundleBeginClass(CompilerBundle* bundle, const char* name)
{
    compilerPrintF(bundle->file, "extern class %s", name);
    bundle->prefix = " : ";
}

void compilerBundleBeginInterface(CompilerBundle* bundle, const char* name)
{
    compilerPrintF(bundle->file, "extern interface %s", name);
    bundle->prefix = " : ";
}

void compilerBundleParentClass(CompilerBundle* bundle, const char* name)
{
    compilerPrintF(bundle->file, "%s%s", bundle->prefix, name);
    bundle->prefix = ", ";
}

void compilerBundleFinalClass(CompilerBundle* bundle)
{
    compilerPrintS(bundle->file, " final");
}

void compilerBundleBeginClassMembers(CompilerBundle* bundle)
{
    compilerPrintS(bundle->file, " {\n");
}

void compilerBundleBeginClassMethod(CompilerBundle* bundle, CompilerVisibility vis)
{
    switch (vis) {
        case COMPILER_PUBLIC: compilerPrintS(bundle->file, "public "); break;
        case COMPILER_PROTECTED: compilerPrintS(bundle->file, "protected "); break;
        case COMPILER_PRIVATE:
        default: assert(false); break;
    }
}

void compilerBundleBeginInterfaceMethod(CompilerBundle* bundle)
{
    UNUSED(bundle);
}

void compilerBundleStaticMethod(CompilerBundle* bundle)
{
    compilerPrintS(bundle->file, "static ");
}

void compilerBundleBeginMethodReturnType(CompilerBundle* bundle)
{
    compilerPrintC(bundle->file, '(');
}

void compilerBundleEndMethodReturnType(CompilerBundle* bundle)
{
    compilerPrintC(bundle->file, ')');
}

void compilerBundleSimpleMethod(CompilerBundle* bundle, const char* name)
{
    compilerPrintC(bundle->file, ' ');
    compilerPrintS(bundle->file, name);
}

void compilerBundleMethodArg(CompilerBundle* bundle, const char* name)
{
    compilerPrintC(bundle->file, ' ');
    compilerPrintS(bundle->file, name);
    compilerPrintS(bundle->file, ":(");
}

void compilerBundleMethodArgName(CompilerBundle* bundle, const char* name)
{
    compilerPrintS(bundle->file, ")");
    compilerPrintS(bundle->file, name);
}

void compilerBundleFinalMethod(CompilerBundle* bundle)
{
    compilerPrintS(bundle->file, " final");
}

void compilerBundleOverrideMethod(CompilerBundle* bundle)
{
    compilerPrintS(bundle->file, " override");
}

void compilerBundleEndMethod(CompilerBundle* bundle)
{
    compilerPrintS(bundle->file, " extern;\n");
}

void compilerBundleEndMethod_Abstract(CompilerBundle* bundle)
{
    compilerPrintS(bundle->file, " abstract;\n");
}

void compilerBundleEndMethod_Extern(CompilerBundle* bundle)
{
    compilerPrintS(bundle->file, " extern;\n");
}

void compilerBundleEndClassOrInterface(CompilerBundle* bundle)
{
    compilerPrintS(bundle->file, "}\n");
}

void compilerBundleType(CompilerBundle* bundle, const char* type)
{
    compilerPrintS(bundle->file, type);
}

void compilerBundleTypePointer(CompilerBundle* bundle)
{
    compilerPrintC(bundle->file, '*');
}
