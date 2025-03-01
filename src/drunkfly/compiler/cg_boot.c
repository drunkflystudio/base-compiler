#include <drunkfly/compiler/codegen.h>
#include <drunkfly/compiler/parser.h>
#include <drunkfly/compiler/private.h>
#include <drunkfly/buff.h>
#include <string.h>

STRUCT(Scope);

STRUCT(Var) {
    Var* next;
    Scope* scope;
    CompilerLocation loc;
    const char* name;
    CompilerType* type;
    bool isConst;
    bool isStatic;
};

struct Scope {
    Scope* parent;
    Var* firstVar;
    Var* lastVar;
    const char* className;
    Buff code;
};

STRUCT(For) {
    For* prev;
    CompilerExpr* init;
    CompilerExpr* condition;
    CompilerExpr* update;
};

STRUCT(Switch) {
    Switch* prev;
    bool inCase;
};

STRUCT(Context) {
    Compiler* compiler;
    CompilerOutputFile* file;
    Scope* scope;
    Buff fwds;
    Buff structs;
    Buff methods;
    const char* currentClass;
    For* currentFor;
    Switch* currentSwitch;
    Var* currentVar;
    bool isConstVariables;
    bool isStaticVariables;
    int indent;
};

/*==================================================================================================================*/

static void fatalError(Context* context, const char* message)
{
    luaL_error(context->compiler->L, message);
}

/*==================================================================================================================*/

static void printLine(Context* context, Buff* buff, const CompilerLocation* loc)
{
    int i;

    if (!context->compiler->getLineNumber || !loc)
        return;

    buffPrintC(buff, '#');
    for (i = 1; i < context->indent * 4; ++i)
        buffPrintC(buff, ' ');

    buffPrintF(buff, "line %d", context->compiler->getLineNumber(loc->startLine));
    if (context->compiler->getFileName) {
        buffPrintS(buff, " \"");
        buffPrintS(buff, context->compiler->getFileName(loc->file));
        buffPrintC(buff, '"');
    }

    buffPrintC(buff, '\n');
}

/*==================================================================================================================*/

struct CompilerType
{
    const char* name;
    CompilerType* pointee;
    CompilerExpr* size;
};

static CompilerType g_void = { "void", NULL, NULL };
static CompilerType g_bool = { "bool", NULL, NULL };
static CompilerType g_int = { "int", NULL, NULL };
static CompilerType g_uint = { "unsigned int", NULL, NULL };
static CompilerType g_intptr = { "intptr_t", NULL, NULL };
static CompilerType g_uintptr = { "uintptr_t", NULL, NULL };
static CompilerType g_int8 = { "int8_t", NULL, NULL };
static CompilerType g_uint8 = { "uint8_t", NULL, NULL };
static CompilerType g_int16 = { "int16_t", NULL, NULL };
static CompilerType g_uint16 = { "uint16_t", NULL, NULL };
static CompilerType g_int32 = { "int32_t", NULL, NULL };
static CompilerType g_uint32 = { "uint32_t", NULL, NULL };
static CompilerType g_string = { "char*", NULL, NULL };
static CompilerType g_object = { "void*", NULL, NULL };

static void printType(Context* context, Buff* buff, CompilerType* type)
{
    CompilerType* pointee = type;
    while (pointee->pointee)
        pointee = pointee->pointee;

    if (pointee->name)
        buffPrintS(buff, pointee->name);

    for (; type->pointee; type = type->pointee)
        buffPrintC(buff, '*');

    UNUSED(context);
}

/*==================================================================================================================*/

struct CompilerExpr
{
    const char* fmt;
    CompilerExpr** operands;
    CompilerLocation loc;
    int numOperands;
};

static void printExpr(Context* context, Buff* buff, CompilerExpr* expr)
{
    const char* p;
    int i = 0;
    for (p = expr->fmt; *p; ++p) {
        if (*p != '@')
            buffPrintC(buff, *p);
        else
            printExpr(context, buff, expr->operands[i++]);
    }
}

static CompilerExpr* newExpr(Context* context, const CompilerLocation* loc, const char* fmt, int numOperands, ...)
{
    va_list args;
    int i;

    CompilerExpr* expr = (CompilerExpr*)compilerTempAlloc(context->compiler, sizeof(CompilerExpr));
    expr->operands = (CompilerExpr**)compilerTempAlloc(context->compiler, sizeof(CompilerExpr*) * (size_t)numOperands);
    expr->fmt = fmt;
    expr->numOperands = numOperands;
    memcpy(&expr->loc, loc, sizeof(CompilerLocation));

    va_start(args, numOperands);
    for (i = 0; i < numOperands; i++)
        expr->operands[i] = va_arg(args, CompilerExpr*);
    va_end(args);

    return expr;
}

/*==================================================================================================================*/

static void pushIndent(Context* context)
{
    ++context->indent;
}

static void popIndent(Context* context)
{
    if (context->indent > 0)
        --context->indent;
    else
        luaL_error(context->compiler->L, "internal error: indent stack underflow.");
}

static void printIndentEx(Context* context, Buff* buff, int adjust)
{
    int i;
    for (i = 0; i < (context->indent + adjust) * 4; ++i)
        buffPrintC(buff, ' ');
}

static void printIndent(Context* context, Buff* buff)
{
    printIndentEx(context, buff, 0);
}

/*==================================================================================================================*/

static void pushScope(Context* context, const char* className)
{
    Scope* scope = (Scope*)compilerTempAlloc(context->compiler, sizeof(Scope));
    scope->firstVar = NULL;
    scope->lastVar = NULL;
    scope->className = className;
    buffInit(&scope->code, context->compiler->L);
    scope->parent = context->scope;
    context->scope = scope;
}

static Scope* popScope(Context* context)
{
    Scope* scope = context->scope;
    if (scope)
        context->scope = scope->parent;
    else
        luaL_error(context->compiler->L, "internal error: scope stack underflow.");
    return scope;
}

static void printVarDecls(Context* context, Scope* scope)
{
    Var* var;
    for (var = scope->firstVar; var; var = var->next) {
        printLine(context, &context->scope->code, &var->loc);
        printIndent(context, &context->scope->code);
        if (var->isStatic)
            buffPrintS(&context->scope->code, "static ");
        printType(context, &context->scope->code, var->type);
        if (var->isConst)
            buffPrintS(&context->scope->code, " const");
        buffPrintC(&context->scope->code, ' ');
        buffPrintS(&context->scope->code, var->name);
        buffPrintS(&context->scope->code, ";\n");
    }
}

static Var* addVariable(Context* context, const CompilerLocation* loc, const char* name)
{
    Scope* scope = context->scope;
    Var* var = NULL;

    if (!scope)
        luaL_error(context->compiler->L, "internal error: scope stack underflow.");
    else {
        var = (Var*)compilerTempAlloc(context->compiler, sizeof(Var));
        memcpy(&var->loc, loc, sizeof(CompilerLocation));
        var->name = compilerTempDupStr(context->compiler, name);
        var->type = NULL;
        var->isConst = context->isConstVariables;
        var->isStatic = context->isStaticVariables;
        var->scope = scope;
        var->next = NULL;

        if (!scope->firstVar)
            scope->firstVar = var;
        else
            scope->lastVar->next = var;
        scope->lastVar = var;
    }

    return var;
}

/*==================================================================================================================*/

static For* pushFor(Context* context)
{
    For* f = (For*)compilerTempAlloc(context->compiler, sizeof(For));
    memset(f, 0, sizeof(For));
    f->prev = context->currentFor;
    context->currentFor = f;
    return f;
}

static For* popFor(Context* context)
{
    For* f = context->currentFor;
    if (f)
        context->currentFor = f->prev;
    else
        luaL_error(context->compiler->L, "internal error: `for` stack underflow.");
    return f;
}

static For* currentFor(Context* context)
{
    For* f = context->currentFor;
    if (!f)
        luaL_error(context->compiler->L, "internal error: `for` stack is empty.");
    return f;
}

/*==================================================================================================================*/

static Switch* pushSwitch(Context* context)
{
    Switch* sw = (Switch*)compilerTempAlloc(context->compiler, sizeof(Switch));
    memset(sw, 0, sizeof(For));
    context->currentSwitch = sw;
    return sw;
}

static Switch* popSwitch(Context* context)
{
    Switch* sw = context->currentSwitch;
    if (sw)
        context->currentSwitch = sw->prev;
    else
        luaL_error(context->compiler->L, "internal error: `switch` stack underflow.");
    return sw;
}

static Switch* currentSwitch(Context* context)
{
    Switch* sw = context->currentSwitch;
    if (!sw)
        luaL_error(context->compiler->L, "internal error: `switch` stack is empty.");
    return sw;
}

static void maybeSwitchStmt(Context* context)
{
    Switch* sw = context->currentSwitch;
    if (!sw || sw->inCase)
        return;

    sw->inCase = true;

    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "{\n");

    pushScope(context, NULL);
    pushIndent(context);
}

static void switchEndCase(Context* context, const CompilerLocation* loc)
{
    Scope* prevScope = popScope(context);

    printVarDecls(context, prevScope);
    buffPrintS(&context->scope->code, buffEnd(&prevScope->code, NULL));
    popIndent(context);

    if (loc)
        printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "}\n");
}

/*==================================================================================================================*/

static void translationUnitBegin(void* ud)
{
    UNUSED(ud);
}

static void translationUnitEnd(void* ud)
{
    Context* context = (Context*)ud;
    compilerPrintS(context->file, "#include <drunkfly/common.h>\n");
    compilerPrintC(context->file, '\n');
    compilerPrintS(context->file, buffEnd(&context->fwds, NULL));
    compilerPrintS(context->file, buffEnd(&context->structs, NULL));
    compilerPrintS(context->file, buffEnd(&context->methods, NULL));
}

static void attrBegin(void* ud, const CompilerLocation* loc, const char* name)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
}

static void attrParam(void* ud, const CompilerLocation* optionalNameLoc, const char* optionalName, CompilerExpr* value)
{
    UNUSED(ud);
    UNUSED(optionalNameLoc);
    UNUSED(optionalName);
    UNUSED(value);
}

static void attrEnd(void* ud)
{
    UNUSED(ud);
}

static void attrListBegin(void* ud)
{
    UNUSED(ud);
}

static void attrListEnd(void* ud)
{
    UNUSED(ud);
}

static void enumBegin(void* ud,
    const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name, bool isFlags)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(nameLoc);
    UNUSED(name);
    UNUSED(isFlags);
}

static void enumType(void* ud, const CompilerLocation* loc, CompilerType* type)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(type);
}

static void enumMembersBegin(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void enumMember(void* ud, const CompilerLocation* loc, const char* name, CompilerExpr* optionalValue)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
    UNUSED(optionalValue);
}

static void enumMembersEnd(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void enumEnd(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void structBegin(void* ud,
    const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name, bool isUnion)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(nameLoc);
    UNUSED(name);
    UNUSED(isUnion);
}

static void structParent(void* ud, const CompilerLocation* loc, const char* name)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
}

static void structMembersBegin(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void structMember(void* ud, const CompilerLocation* loc, const char* name, CompilerType* type)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
    UNUSED(type);
}

static void structMembersEnd(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void structEnd(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void classBegin(void* ud,
    const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name, bool isFinal)
{
    Context* context = (Context*)ud;
    UNUSED(isFinal);
    UNUSED(loc);

    context->currentClass = name;

    printLine(context, &context->fwds, nameLoc);
    buffPrintF(&context->fwds, "STRUCT(%s);\n", name);

    buffPrintC(&context->structs, '\n');
    printLine(context, &context->structs, nameLoc);
    buffPrintF(&context->structs, "struct %s\n", name);
}

static void classInterfaceBegin(void* ud,
    const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name)
{
    Context* context = (Context*)ud;
    UNUSED(loc);

    context->currentClass = name;

    printLine(context, &context->fwds, nameLoc);
    buffPrintF(&context->fwds, "struct %s; typedef struct %s %s;\n", name, name, name);

    buffPrintC(&context->structs, '\n');
    printLine(context, &context->structs, nameLoc);
    buffPrintF(&context->structs, "struct %s\n", name);
}

static void classParent(void* ud, const CompilerLocation* loc, const char* name)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
}

static void classMembersBegin(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    pushScope(context, context->currentClass);
    printLine(context, &context->structs, loc);
    buffPrintS(&context->structs, "{\n");

    printLine(context, &context->structs, loc);
    printIndentEx(context, &context->structs, 1);
    buffPrintS(&context->structs, "int (*dispatch)(lua_State* L);\n");
}

static void classFriend(void* ud, const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(nameLoc);
    UNUSED(name);
}

static void classVariablesBegin(void* ud, const CompilerLocation* loc,
    const CompilerLocation* visLoc, CompilerVisibility vis, const CompilerLocation* optionalStatic, bool isConst)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(visLoc);
    UNUSED(vis);
    UNUSED(optionalStatic);
    UNUSED(isConst);
}

static void classVariablesEnd(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void classDestructorBegin(void* ud,
    const CompilerLocation* loc, const char* name, const CompilerLocation* visLoc, CompilerVisibility vis)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
    UNUSED(visLoc);
    UNUSED(vis);
}

static void classDestructorEnd(void* ud)
{
    UNUSED(ud);
}

static void classMethodBegin(void* ud, const CompilerLocation* loc, const CompilerLocation* optionalVisLoc,
    CompilerVisibility vis, const CompilerLocation* optionalStatic, const CompilerLocation* retLoc, CompilerType* ret)
{
    Context* context = (Context*)ud;
    UNUSED(ret);
    UNUSED(loc);
    UNUSED(optionalVisLoc);
    UNUSED(vis);
    UNUSED(optionalStatic);

    buffPrintC(&context->methods, '\n');
    printLine(context, &context->methods, retLoc);
    buffPrintF(&context->methods, "void\n");

    pushScope(context, NULL);
}

static void classMethodNameSimple(void* ud, const CompilerLocation* loc, const char* name)
{
    Context* context = (Context*)ud;
    printLine(context, &context->methods, loc);
    buffPrintF(&context->methods, "%s_%s(lua_State* L) { UNUSED(L);\n", context->currentClass, name);
}

static void classMethodNameArg(void* ud, const CompilerLocation* loc,
    const char* name, CompilerType* type, const CompilerLocation* argLoc, const char* arg)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
    UNUSED(type);
    UNUSED(argLoc);
    UNUSED(arg);
}

static void classMethodNameEnd(void* ud,
    const CompilerLocation* optionalFinal, const CompilerLocation* optionalOverride)
{
    UNUSED(ud);
    UNUSED(optionalFinal);
    UNUSED(optionalOverride);
}

static void classMethodEnd_Abstract(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void classMethodEnd_Extern(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void classMethodBodyBegin(void* ud)
{
    UNUSED(ud);
}

static void classMethodEnd(void* ud)
{
    Context* context = (Context*)ud;
    Scope* scope = popScope(context);
    printVarDecls(context, scope);
    buffPrintS(&context->methods, buffEnd(&scope->code, NULL));
    buffPrintS(&context->methods, "}\n");
}

static void classMembersEnd(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    popScope(context);
    UNUSED(loc);
}

static void classEnd(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    printLine(context, &context->structs, loc);
    buffPrintS(&context->structs, "};\n");
    context->currentClass = NULL;
}

static void varDeclBegin(void* ud, const CompilerLocation* loc, const CompilerLocation* optionalStatic, bool isConst)
{
    Context* context = (Context*)ud;
    maybeSwitchStmt(context);
    context->isConstVariables = isConst;
    context->isStaticVariables = (optionalStatic != NULL);
    UNUSED(loc);
}

static void varBegin(void* ud, const CompilerLocation* loc, const char* name)
{
    Context* context = (Context*)ud;
    context->currentVar = addVariable(context, loc, name);
}

static void varType(void* ud, const CompilerLocation* loc, CompilerType* type)
{
    Context* context = (Context*)ud;
    context->currentVar->type = type;
    UNUSED(loc);
}

static void varType_Array(void* ud, const CompilerLocation* loc, CompilerType* elementType, CompilerExpr* size)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(elementType);
    UNUSED(size);
}

static void varInitializer(void* ud, CompilerExpr* value)
{
    Context* context = (Context*)ud;
    printLine(context, &context->scope->code, &value->loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "%s = ", context->currentVar->name);
    printExpr(context, &context->scope->code, value);
    buffPrintS(&context->scope->code, ";\n");
}

static void varEnd(void* ud)
{
    Context* context = (Context*)ud;
    if (context->currentVar->type == NULL) {
        luaL_error(context->compiler->L,
            "variable \"%s\": untyped variables are not supported by bootstrap backend.", context->currentVar->name);
    }
    context->currentVar = NULL;
}

static void varDeclEnd(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void structInitializerBegin(void* ud)
{
    UNUSED(ud);
}

static void structInitializer(void* ud, const CompilerLocation* loc, const char* name, CompilerExpr* value)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
    UNUSED(value);
    /* FIXME */
}

static void structInitializerEnd(void* ud)
{
    UNUSED(ud);
}

static void arrayInitializerBegin(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void arrayInitializer(void* ud, CompilerExpr* value)
{
    UNUSED(ud);
    UNUSED(value);
}

static void arrayInitializerEnd(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static CompilerType* typeVoid(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_void;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4702)
#endif

static CompilerType* typeBit(void* ud, const CompilerLocation* loc, CompilerExpr* optionalExpr)
{
    Context* context = (Context*)ud;
    UNUSED(loc);
    UNUSED(optionalExpr);
    fatalError(context, "bit type is not supported in bootstrap code generator.");

    return NULL;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

static CompilerType* typeBool(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_bool;
}

static CompilerType* typeInt(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_int;
}

static CompilerType* typeUInt(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_uint;
}

static CompilerType* typeIntPtr(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_intptr;
}

static CompilerType* typeUIntPtr(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_uintptr;
}

static CompilerType* typeInt8(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_int8;
}

static CompilerType* typeUInt8(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_uint8;
}

static CompilerType* typeInt16(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_int16;
}

static CompilerType* typeUInt16(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_uint16;
}

static CompilerType* typeInt32(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_int32;
}

static CompilerType* typeUInt32(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_uint32;
}

static CompilerType* typeString(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_string;
}

static CompilerType* typeObject(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_object;
}

static CompilerType* typeIdentifier(void* ud, const CompilerLocation* loc, const char* text)
{
    Context* ctx = (Context*)ud;
    CompilerType* type = (CompilerType*)compilerTempAlloc(ctx->compiler, sizeof(CompilerType));
    memset(type, 0, sizeof(CompilerType));
    type->name = compilerTempDupStr(ctx->compiler, text);
    UNUSED(loc);
    return type;
}

static CompilerType* typePointer(void* ud, const CompilerLocation* loc, CompilerType* pointee)
{
    Context* ctx = (Context*)ud;
    CompilerType* type = (CompilerType*)compilerTempAlloc(ctx->compiler, sizeof(CompilerType));
    memset(type, 0, sizeof(CompilerType));
    type->pointee = pointee;
    UNUSED(loc);
    return type;
}

static CompilerExpr* exprNull(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "null", 0);
}

static CompilerExpr* exprFalse(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "false", 0);
}

static CompilerExpr* exprTrue(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "true", 0);
}

static CompilerExpr* exprIdentifier(void* ud, const CompilerLocation* loc, const char* name)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, name, 0);
}

static CompilerExpr* exprInteger(void* ud, const CompilerLocation* loc, uint_value_t value)
{
    Context* context = (Context*)ud;
    const char* text = compilerPushHexString(context->compiler->L, value);
    return newExpr(context, loc, text, 0);
}

static CompilerExpr* exprParentheses(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "(@)", 1, operand);
}

static void exprNewBegin(void* ud, const CompilerLocation* loc, CompilerType* type)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(type);
    /* FIXME */
}

static CompilerExpr* exprNewEnd_Struct(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return NULL;
}

static CompilerExpr* exprNewEnd_Array(void* ud, const CompilerLocation* loc, CompilerExpr* size)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(size);
    return NULL;
}

static void exprMethodCallBegin(void* ud, CompilerExpr* callee)
{
    UNUSED(ud);
    UNUSED(callee);
    /* FIXME */
}

static void exprMethodSimple(void* ud, const CompilerLocation* loc, const char* name)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
}

static void exprMethodArg(void* ud, const CompilerLocation* loc, const char* name, CompilerExpr* value)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
    UNUSED(value);
}

static CompilerExpr* exprMethodCallEnd(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return NULL;
}

static CompilerExpr* exprSubscript(void* ud, const CompilerLocation* loc, CompilerExpr* arr, CompilerExpr* idx)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@[@]", 2, arr, idx);
}

static CompilerExpr* exprMember(void* ud,
    const CompilerLocation* loc, CompilerExpr* target, const CompilerLocation* nameLoc, const char* name)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(target);
    UNUSED(nameLoc);
    UNUSED(name);
    /* FIXME */
    return NULL;
}

static CompilerExpr* exprPostfixIncr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@++", 1, operand);
}

static CompilerExpr* exprPostfixDecr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@--", 1, operand);
}

static CompilerExpr* exprPrefixIncr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "++@", 1, operand);
}

static CompilerExpr* exprPrefixDecr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "--@", 1, operand);
}

static CompilerExpr* exprTakeAddress(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "&@", 1, operand);
}

static CompilerExpr* exprDeref(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "*@", 1, operand);
}

static CompilerExpr* exprUnaryPlus(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@", 1, operand);
}

static CompilerExpr* exprUnaryMinus(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "-@", 1, operand);
}

static CompilerExpr* exprBitwiseNot(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "~@", 1, operand);
}

static CompilerExpr* exprLogicNot(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "!@", 1, operand);
}

static CompilerExpr* exprSizeOf(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "sizeof(@)", 1, operand);
}

static CompilerExpr* exprMultiply(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ * @", 2, op1, op2);
}

static CompilerExpr* exprDivide(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ / @", 2, op1, op2);
}

static CompilerExpr* exprModulo(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ % @", 2, op1, op2);
}

static CompilerExpr* exprAdd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ + @", 2, op1, op2);
}

static CompilerExpr* exprSubtract(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ - @", 2, op1, op2);
}

static CompilerExpr* exprShiftLeft(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ << @", 2, op1, op2);
}

static CompilerExpr* exprShiftRight(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ >> @", 2, op1, op2);
}

static CompilerExpr* exprGreater(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ > @", 2, op1, op2);
}

static CompilerExpr* exprGreaterEq(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ >= @", 2, op1, op2);
}

static CompilerExpr* exprLess(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ < @", 2, op1, op2);
}

static CompilerExpr* exprLessEq(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ <= @", 2, op1, op2);
}

static CompilerExpr* exprEqual(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ == @", 2, op1, op2);
}

static CompilerExpr* exprNotEqual(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ != @", 2, op1, op2);
}

static CompilerExpr* exprBitwiseAnd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ & @", 2, op1, op2);
}

static CompilerExpr* exprBitwiseXor(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ ^ @", 2, op1, op2);
}

static CompilerExpr* exprBitwiseOr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ | @", 2, op1, op2);
}

static CompilerExpr* exprLogicAnd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ && @", 2, op1, op2);
}

static CompilerExpr* exprLogicOr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ || @", 2, op1, op2);
}

static CompilerExpr* exprTernary(void* ud, const CompilerLocation* loc,
    CompilerExpr* cond, CompilerExpr* trueValue, CompilerExpr* falseValue)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ ? @ : @", 3, cond, trueValue, falseValue);
}

static CompilerExpr* exprAssign(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ = @", 2, op1, op2);
}

static CompilerExpr* exprAssignAdd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ += @", 2, op1, op2);
}

static CompilerExpr* exprAssignSub(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ -= @", 2, op1, op2);
}

static CompilerExpr* exprAssignMul(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ *= @", 2, op1, op2);
}

static CompilerExpr* exprAssignDiv(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ /= @", 2, op1, op2);
}

static CompilerExpr* exprAssignMod(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ %= @", 2, op1, op2);
}

static CompilerExpr* exprAssignAnd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ &= @", 2, op1, op2);
}

static CompilerExpr* exprAssignOr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ |= @", 2, op1, op2);
}

static CompilerExpr* exprAssignXor(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ ^= @", 2, op1, op2);
}

static CompilerExpr* exprAssignShl(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ <<= @", 2, op1, op2);
}

static CompilerExpr* exprAssignShr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    Context* context = (Context*)ud;
    return newExpr(context, loc, "@ >>= @", 2, op1, op2);
}

static void stmtEmpty(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    maybeSwitchStmt(context);
    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, ";\n");
}

static void stmtExpr(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    Context* context = (Context*)ud;
    maybeSwitchStmt(context);
    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    printExpr(context, &context->scope->code, e);
    buffPrintS(&context->scope->code, ";\n");
}

static void stmtLabel(void* ud, const CompilerLocation* loc, const char* name)
{
    Context* context = (Context*)ud;
    maybeSwitchStmt(context);
    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "%s:;\n", name);
}

static void stmtGoto(void* ud, const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name)
{
    Context* context = (Context*)ud;
    maybeSwitchStmt(context);
    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "goto %s;\n", name);
    UNUSED(nameLoc);
}

static void stmtBreak(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    maybeSwitchStmt(context);
    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "break;\n");
}

static void stmtContinue(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    maybeSwitchStmt(context);
    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "continue;\n");
}

static void stmtDelete(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(e);

    /* FIXME */
}

static void stmtThrow(void* ud, const CompilerLocation* loc, CompilerExpr* optionalExpr)
{
    Context* context = (Context*)ud;

    UNUSED(loc);
    UNUSED(optionalExpr);

    luaL_error(context->compiler->L, "throw is not supported by bootstrap backend.");
}

static void stmtReturn(void* ud, const CompilerLocation* loc, CompilerExpr* optionalExpr)
{
    Context* context = (Context*)ud;
    maybeSwitchStmt(context);
    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "return ");
    if (optionalExpr)
        printExpr(context, &context->scope->code, optionalExpr);
    buffPrintF(&context->scope->code, ";\n");
}

static void stmtCompoundBegin(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    maybeSwitchStmt(context);
    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "{\n");
    pushScope(context, NULL);
    pushIndent(context);
}

static void stmtCompoundEnd(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    Scope* prevScope = popScope(context);

    printVarDecls(context, prevScope);
    buffPrintS(&context->scope->code, buffEnd(&prevScope->code, NULL));

    popIndent(context);

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "}\n");
}

static void stmtIfBegin(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    maybeSwitchStmt(context);

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "{\n");

    pushScope(context, NULL);
    pushIndent(context);
}

static void stmtIfThen(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    Context* context = (Context*)ud;

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "if (");
    printExpr(context, &context->scope->code, e);
    buffPrintF(&context->scope->code, ") {\n");

    pushScope(context, NULL);
    pushIndent(context);
}

static void stmtIfElse(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    Scope* prevScope = popScope(context);
    popIndent(context);

    printVarDecls(context, prevScope);
    buffPrintS(&context->scope->code, buffEnd(&prevScope->code, NULL));

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "} else {\n");

    pushScope(context, NULL);
    pushIndent(context);
}

static void stmtIfEnd(void* ud)
{
    Context* context = (Context*)ud;
    Scope* prevScope;

    prevScope = popScope(context);
    printVarDecls(context, prevScope);
    buffPrintS(&context->scope->code, buffEnd(&prevScope->code, NULL));

    popIndent(context);

    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "}\n");

    prevScope = popScope(context);
    printVarDecls(context, prevScope);
    buffPrintS(&context->scope->code, buffEnd(&prevScope->code, NULL));

    popIndent(context);

    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "}\n");
}

static void stmtWhileBegin(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    maybeSwitchStmt(context);

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "{\n");

    pushScope(context, NULL);
    pushIndent(context);
}

static void stmtWhileDo(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    Context* context = (Context*)ud;

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "while (");
    printExpr(context, &context->scope->code, e);
    buffPrintF(&context->scope->code, ") {\n");

    pushScope(context, NULL);
    pushIndent(context);
}

static void stmtWhileEnd(void* ud)
{
    Context* context = (Context*)ud;
    Scope* prevScope;

    prevScope = popScope(context);
    printVarDecls(context, prevScope);
    buffPrintS(&context->scope->code, buffEnd(&prevScope->code, NULL));

    popIndent(context);

    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "}\n");

    prevScope = popScope(context);
    printVarDecls(context, prevScope);
    buffPrintS(&context->scope->code, buffEnd(&prevScope->code, NULL));

    popIndent(context);

    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "}\n");
}

static void stmtDoBegin(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    maybeSwitchStmt(context);

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "{\n");

    pushScope(context, NULL);
    pushIndent(context);

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "do {\n");

    pushScope(context, NULL);
    pushIndent(context);
}

static void stmtDoEnd(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    Context* context = (Context*)ud;
    Scope* prevScope1, *prevScope2;

    prevScope1 = popScope(context);
    buffPrintS(&context->scope->code, buffEnd(&prevScope1->code, NULL));
    popIndent(context);

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "} while (");
    printExpr(context, &context->scope->code, e);
    buffPrintS(&context->scope->code, ");\n");

    prevScope2 = popScope(context);
    printVarDecls(context, prevScope1);
    printVarDecls(context, prevScope2);
    buffPrintS(&context->scope->code, buffEnd(&prevScope2->code, NULL));
    popIndent(context);

    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "}\n");
}

static void stmtForBegin(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    maybeSwitchStmt(context);

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "{\n");

    pushFor(context);
    pushScope(context, NULL);
    pushIndent(context);
}

static void stmtForInit(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    Context* context = (Context*)ud;
    For* f;

    UNUSED(loc);

    f = currentFor(context);
    f->init = e;
}

static void stmtForCondition(void* ud, CompilerExpr* e)
{
    Context* context = (Context*)ud;
    currentFor(context)->condition = e;
}

static void stmtForUpdate(void* ud, CompilerExpr* e)
{
    Context* context = (Context*)ud;
    currentFor(context)->update = e;
}

static void stmtForDo(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    For* f = popFor(context);

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "for (");
    if (f->init)
        printExpr(context, &context->scope->code, f->init);
    buffPrintF(&context->scope->code, "; ");
    if (f->condition)
        printExpr(context, &context->scope->code, f->condition);
    buffPrintF(&context->scope->code, "; ");
    if (f->update)
        printExpr(context, &context->scope->code, f->update);
    buffPrintF(&context->scope->code, ") {\n");

    pushScope(context, NULL);
    pushIndent(context);
}

static void stmtForEnd(void* ud)
{
    Context* context = (Context*)ud;
    Scope* prevScope;

    prevScope = popScope(context);
    printVarDecls(context, prevScope);
    buffPrintS(&context->scope->code, buffEnd(&prevScope->code, NULL));

    popIndent(context);

    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "}\n");

    prevScope = popScope(context);
    printVarDecls(context, prevScope);
    buffPrintS(&context->scope->code, buffEnd(&prevScope->code, NULL));

    popIndent(context);

    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "}\n");
}

static void stmtSwitchBegin(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    maybeSwitchStmt(context);

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "{\n");

    pushScope(context, NULL);
    pushIndent(context);
}

static void stmtSwitchOperand(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    Context* context = (Context*)ud;

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "switch (");
    printExpr(context, &context->scope->code, e);
    buffPrintF(&context->scope->code, ") {\n");

    pushSwitch(context);
    pushScope(context, NULL);
    pushIndent(context);
}

static void stmtSwitchCase(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    Context* context = (Context*)ud;
    Switch* sw = currentSwitch(context);

    if (sw->inCase) {
        sw->inCase = false;
        switchEndCase(context, loc);
    }

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "case ");
    printExpr(context, &context->scope->code, e);
    buffPrintF(&context->scope->code, ":\n");
}

static void stmtSwitchCaseRange(void* ud, const CompilerLocation* loc, CompilerExpr* start, CompilerExpr* end)
{
    Context* context = (Context*)ud;

    UNUSED(loc);
    UNUSED(start);
    UNUSED(end);

    luaL_error(context->compiler->L, "case with range is not supported by bootstrap backend.");
}

static void stmtSwitchDefault(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    Switch* sw = currentSwitch(context);

    if (sw->inCase) {
        sw->inCase = false;
        switchEndCase(context, loc);
    }

    printLine(context, &context->scope->code, loc);
    printIndent(context, &context->scope->code);
    buffPrintF(&context->scope->code, "default:\n");
}

static void stmtSwitchEnd(void* ud)
{
    Context* context = (Context*)ud;
    Switch* sw = popSwitch(context);
    Scope* prevScope;

    if (sw->inCase) {
        sw->inCase = false;
        switchEndCase(context, NULL);
    }

    prevScope = popScope(context);
    printVarDecls(context, prevScope);
    buffPrintS(&context->scope->code, buffEnd(&prevScope->code, NULL));
    popIndent(context);

    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "}\n");

    prevScope = popScope(context);
    printVarDecls(context, prevScope);
    buffPrintS(&context->scope->code, buffEnd(&prevScope->code, NULL));
    popIndent(context);

    printIndent(context, &context->scope->code);
    buffPrintS(&context->scope->code, "}\n");
}

static void stmtTryBegin(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;

    UNUSED(loc);

    luaL_error(context->compiler->L, "try/catch is not supported by bootstrap backend.");
}

static void stmtTry_CatchBegin(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;

    UNUSED(loc);

    luaL_error(context->compiler->L, "try/catch is not supported by bootstrap backend.");
}

static void stmtTry_CatchDo(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;

    UNUSED(loc);

    luaL_error(context->compiler->L, "try/catch is not supported by bootstrap backend.");
}

static void stmtTry_CatchEnd(void* ud)
{
    Context* context = (Context*)ud;
    luaL_error(context->compiler->L, "try/catch is not supported by bootstrap backend.");
}

static void stmtTry_FinallyBegin(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;

    UNUSED(loc);

    luaL_error(context->compiler->L, "try/catch is not supported by bootstrap backend.");
}

static void stmtTry_FinallyEnd(void* ud)
{
    Context* context = (Context*)ud;
    luaL_error(context->compiler->L, "try/catch is not supported by bootstrap backend.");
}

static void stmtTryEnd(void* ud)
{
    Context* context = (Context*)ud;
    luaL_error(context->compiler->L, "try/catch is not supported by bootstrap backend.");
}

static void error(void* ud,
    const CompilerLocation* loc, const CompilerToken* token, const char* message, const char* detail)
{
    Context* context = (Context*)ud;
    UNUSED(token);

    if (!context->compiler->getLineNumber || !loc)
        luaL_error(context->compiler->L, "%s: %s.\n", message, detail);
    else {
        if (!context->compiler->getFileName) {
            luaL_error(context->compiler->L, "(%d,%d): %s: %s.\n",
                context->compiler->getLineNumber(loc->startLine),
                loc->startColumn, message, detail);
        } else {
            luaL_error(context->compiler->L, "%s(%d,%d): %s: %s.\n",
                context->compiler->getFileName(loc->file),
                context->compiler->getLineNumber(loc->startLine),
                loc->startColumn, message, detail);
        }
    }
}

/*==================================================================================================================*/

void compilerInitBootstrapCodegen(Compiler* compiler, const char* outputFile)
{
    Context* context = (Context*)compilerTempAlloc(compiler, sizeof(Context));
    memset(context, 0, sizeof(Context));
    context->compiler = compiler;
    context->file = compilerBeginOutput(compiler, outputFile);
    buffInit(&context->fwds, compiler->L);
    buffInit(&context->structs, compiler->L);
    buffInit(&context->methods, compiler->L);
    compiler->parser.cb.ud = context;
    compiler->parser.cb.translationUnitBegin = translationUnitBegin;
    compiler->parser.cb.translationUnitEnd = translationUnitEnd;
    compiler->parser.cb.attrBegin = attrBegin;
    compiler->parser.cb.attrParam = attrParam;
    compiler->parser.cb.attrEnd = attrEnd;
    compiler->parser.cb.attrListBegin = attrListBegin;
    compiler->parser.cb.attrListEnd = attrListEnd;
    compiler->parser.cb.enumBegin = enumBegin;
    compiler->parser.cb.enumType = enumType;
    compiler->parser.cb.enumMembersBegin = enumMembersBegin;
    compiler->parser.cb.enumMember = enumMember;
    compiler->parser.cb.enumMembersEnd = enumMembersEnd;
    compiler->parser.cb.enumEnd = enumEnd;
    compiler->parser.cb.structBegin = structBegin;
    compiler->parser.cb.structParent = structParent;
    compiler->parser.cb.structMembersBegin = structMembersBegin;
    compiler->parser.cb.structMember = structMember;
    compiler->parser.cb.structMembersEnd = structMembersEnd;
    compiler->parser.cb.structEnd = structEnd;
    compiler->parser.cb.structBegin = structBegin;
    compiler->parser.cb.structParent = structParent;
    compiler->parser.cb.structMembersBegin = structMembersBegin;
    compiler->parser.cb.structMember = structMember;
    compiler->parser.cb.structMembersEnd = structMembersEnd;
    compiler->parser.cb.structEnd = structEnd;
    compiler->parser.cb.classBegin = classBegin;
    compiler->parser.cb.classInterfaceBegin = classInterfaceBegin;
    compiler->parser.cb.classParent = classParent;
    compiler->parser.cb.classMembersBegin = classMembersBegin;
    compiler->parser.cb.classFriend = classFriend;
    compiler->parser.cb.classVariablesBegin = classVariablesBegin;
    compiler->parser.cb.classVariablesEnd = classVariablesEnd;
    compiler->parser.cb.classDestructorBegin = classDestructorBegin;
    compiler->parser.cb.classDestructorEnd = classDestructorEnd;
    compiler->parser.cb.classMethodBegin = classMethodBegin;
    compiler->parser.cb.classMethodNameSimple = classMethodNameSimple;
    compiler->parser.cb.classMethodNameArg = classMethodNameArg;
    compiler->parser.cb.classMethodNameEnd = classMethodNameEnd;
    compiler->parser.cb.classMethodEnd_Abstract = classMethodEnd_Abstract;
    compiler->parser.cb.classMethodEnd_Extern = classMethodEnd_Extern;
    compiler->parser.cb.classMethodBodyBegin = classMethodBodyBegin;
    compiler->parser.cb.classMethodEnd = classMethodEnd;
    compiler->parser.cb.classMembersEnd = classMembersEnd;
    compiler->parser.cb.classEnd = classEnd;
    compiler->parser.cb.varDeclBegin = varDeclBegin;
    compiler->parser.cb.varBegin = varBegin;
    compiler->parser.cb.varType = varType;
    compiler->parser.cb.varType_Array = varType_Array;
    compiler->parser.cb.varInitializer = varInitializer;
    compiler->parser.cb.varEnd = varEnd;
    compiler->parser.cb.varDeclEnd = varDeclEnd;
    compiler->parser.cb.structInitializerBegin = structInitializerBegin;
    compiler->parser.cb.structInitializer = structInitializer;
    compiler->parser.cb.structInitializerEnd = structInitializerEnd;
    compiler->parser.cb.arrayInitializerBegin = arrayInitializerBegin;
    compiler->parser.cb.arrayInitializer = arrayInitializer;
    compiler->parser.cb.arrayInitializerEnd = arrayInitializerEnd;
    compiler->parser.cb.typeVoid = typeVoid;
    compiler->parser.cb.typeBit = typeBit;
    compiler->parser.cb.typeBool = typeBool;
    compiler->parser.cb.typeInt = typeInt;
    compiler->parser.cb.typeUInt = typeUInt;
    compiler->parser.cb.typeIntPtr = typeIntPtr;
    compiler->parser.cb.typeUIntPtr = typeUIntPtr;
    compiler->parser.cb.typeInt8 = typeInt8;
    compiler->parser.cb.typeUInt8 = typeUInt8;
    compiler->parser.cb.typeInt16 = typeInt16;
    compiler->parser.cb.typeUInt16 = typeUInt16;
    compiler->parser.cb.typeInt32 = typeInt32;
    compiler->parser.cb.typeUInt32 = typeUInt32;
    compiler->parser.cb.typeString = typeString;
    compiler->parser.cb.typeObject = typeObject;
    compiler->parser.cb.typeIdentifier = typeIdentifier;
    compiler->parser.cb.typePointer = typePointer;
    compiler->parser.cb.exprNull = exprNull;
    compiler->parser.cb.exprFalse = exprFalse;
    compiler->parser.cb.exprTrue = exprTrue;
    compiler->parser.cb.exprIdentifier = exprIdentifier;
    compiler->parser.cb.exprInteger = exprInteger;
    compiler->parser.cb.exprParentheses = exprParentheses;
    compiler->parser.cb.exprNewBegin = exprNewBegin;
    compiler->parser.cb.exprNewEnd_Struct = exprNewEnd_Struct;
    compiler->parser.cb.exprNewEnd_Array = exprNewEnd_Array;
    compiler->parser.cb.exprMethodCallBegin = exprMethodCallBegin;
    compiler->parser.cb.exprMethodSimple = exprMethodSimple;
    compiler->parser.cb.exprMethodArg = exprMethodArg;
    compiler->parser.cb.exprMethodCallEnd = exprMethodCallEnd;
    compiler->parser.cb.exprSubscript = exprSubscript;
    compiler->parser.cb.exprMember = exprMember;
    compiler->parser.cb.exprPostfixIncr = exprPostfixIncr;
    compiler->parser.cb.exprPostfixDecr = exprPostfixDecr;
    compiler->parser.cb.exprPrefixIncr = exprPrefixIncr;
    compiler->parser.cb.exprPrefixDecr = exprPrefixDecr;
    compiler->parser.cb.exprTakeAddress = exprTakeAddress;
    compiler->parser.cb.exprDeref = exprDeref;
    compiler->parser.cb.exprUnaryPlus = exprUnaryPlus;
    compiler->parser.cb.exprUnaryMinus = exprUnaryMinus;
    compiler->parser.cb.exprBitwiseNot = exprBitwiseNot;
    compiler->parser.cb.exprLogicNot = exprLogicNot;
    compiler->parser.cb.exprSizeOf = exprSizeOf;
    compiler->parser.cb.exprMultiply = exprMultiply;
    compiler->parser.cb.exprDivide = exprDivide;
    compiler->parser.cb.exprModulo = exprModulo;
    compiler->parser.cb.exprAdd = exprAdd;
    compiler->parser.cb.exprSubtract = exprSubtract;
    compiler->parser.cb.exprShiftLeft = exprShiftLeft;
    compiler->parser.cb.exprShiftRight = exprShiftRight;
    compiler->parser.cb.exprGreater = exprGreater;
    compiler->parser.cb.exprGreaterEq = exprGreaterEq;
    compiler->parser.cb.exprLess = exprLess;
    compiler->parser.cb.exprLessEq = exprLessEq;
    compiler->parser.cb.exprEqual = exprEqual;
    compiler->parser.cb.exprNotEqual = exprNotEqual;
    compiler->parser.cb.exprBitwiseAnd = exprBitwiseAnd;
    compiler->parser.cb.exprBitwiseXor = exprBitwiseXor;
    compiler->parser.cb.exprBitwiseOr = exprBitwiseOr;
    compiler->parser.cb.exprLogicAnd = exprLogicAnd;
    compiler->parser.cb.exprLogicOr = exprLogicOr;
    compiler->parser.cb.exprTernary = exprTernary;
    compiler->parser.cb.exprAssign = exprAssign;
    compiler->parser.cb.exprAssignAdd = exprAssignAdd;
    compiler->parser.cb.exprAssignSub = exprAssignSub;
    compiler->parser.cb.exprAssignMul = exprAssignMul;
    compiler->parser.cb.exprAssignDiv = exprAssignDiv;
    compiler->parser.cb.exprAssignMod = exprAssignMod;
    compiler->parser.cb.exprAssignAnd = exprAssignAnd;
    compiler->parser.cb.exprAssignOr = exprAssignOr;
    compiler->parser.cb.exprAssignXor = exprAssignXor;
    compiler->parser.cb.exprAssignShl = exprAssignShl;
    compiler->parser.cb.exprAssignShr = exprAssignShr;
    compiler->parser.cb.stmtEmpty = stmtEmpty;
    compiler->parser.cb.stmtExpr = stmtExpr;
    compiler->parser.cb.stmtLabel = stmtLabel;
    compiler->parser.cb.stmtGoto = stmtGoto;
    compiler->parser.cb.stmtBreak = stmtBreak;
    compiler->parser.cb.stmtContinue = stmtContinue;
    compiler->parser.cb.stmtDelete = stmtDelete;
    compiler->parser.cb.stmtThrow = stmtThrow;
    compiler->parser.cb.stmtReturn = stmtReturn;
    compiler->parser.cb.stmtCompoundBegin = stmtCompoundBegin;
    compiler->parser.cb.stmtCompoundEnd = stmtCompoundEnd;
    compiler->parser.cb.stmtIfBegin = stmtIfBegin;
    compiler->parser.cb.stmtIfThen = stmtIfThen;
    compiler->parser.cb.stmtIfElse = stmtIfElse;
    compiler->parser.cb.stmtIfEnd = stmtIfEnd;
    compiler->parser.cb.stmtWhileBegin = stmtWhileBegin;
    compiler->parser.cb.stmtWhileDo = stmtWhileDo;
    compiler->parser.cb.stmtWhileEnd = stmtWhileEnd;
    compiler->parser.cb.stmtDoBegin = stmtDoBegin;
    compiler->parser.cb.stmtDoEnd = stmtDoEnd;
    compiler->parser.cb.stmtForBegin = stmtForBegin;
    compiler->parser.cb.stmtForInit = stmtForInit;
    compiler->parser.cb.stmtForCondition = stmtForCondition;
    compiler->parser.cb.stmtForUpdate = stmtForUpdate;
    compiler->parser.cb.stmtForDo = stmtForDo;
    compiler->parser.cb.stmtForEnd = stmtForEnd;
    compiler->parser.cb.stmtSwitchBegin = stmtSwitchBegin;
    compiler->parser.cb.stmtSwitchOperand = stmtSwitchOperand;
    compiler->parser.cb.stmtSwitchCase = stmtSwitchCase;
    compiler->parser.cb.stmtSwitchCaseRange = stmtSwitchCaseRange;
    compiler->parser.cb.stmtSwitchDefault = stmtSwitchDefault;
    compiler->parser.cb.stmtSwitchEnd = stmtSwitchEnd;
    compiler->parser.cb.stmtTryBegin = stmtTryBegin;
    compiler->parser.cb.stmtTry_CatchBegin = stmtTry_CatchBegin;
    compiler->parser.cb.stmtTry_CatchDo = stmtTry_CatchDo;
    compiler->parser.cb.stmtTry_CatchEnd = stmtTry_CatchEnd;
    compiler->parser.cb.stmtTry_FinallyBegin = stmtTry_FinallyBegin;
    compiler->parser.cb.stmtTry_FinallyEnd = stmtTry_FinallyEnd;
    compiler->parser.cb.stmtTryEnd = stmtTryEnd;
    compiler->parser.cb.error = error;
}
