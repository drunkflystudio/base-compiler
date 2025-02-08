#include <drunkfly/compiler/codegen.h>
#include <drunkfly/compiler/parser.h>
#include <drunkfly/compiler/private.h>
#include <drunkfly/buff.h>

STRUCT(Context) {
    Compiler* compiler;
    CompilerOutputFile* file;
    Buff fwds;
    Buff structs;
    Buff methods;
    const char* currentClass;
};

/*==================================================================================================================*/

static void printLine(Context* context, Buff* buff, const CompilerLocation* loc)
{
    /* FIXME */
    UNUSED(context);
    UNUSED(buff);
    UNUSED(loc);
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
static CompilerType g_int8 = { "int8_t", NULL, NULL };
static CompilerType g_uint8 = { "uint8_t", NULL, NULL };
static CompilerType g_int16 = { "int16_t", NULL, NULL };
static CompilerType g_uint16 = { "uint16_t", NULL, NULL };
static CompilerType g_int32 = { "int32_t", NULL, NULL };
static CompilerType g_uint32 = { "uint32_t", NULL, NULL };
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
    buffPrintF(&context->fwds, "struct %s; typedef struct %s %s;\n", name, name, name);

    buffPrintC(&context->structs, '\n');
    printLine(context, &context->structs, nameLoc);
    buffPrintF(&context->structs, "struct %s\n", name);
}

static void classBeginInterface(void* ud,
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
    printLine(context, &context->structs, loc);
    buffPrintS(&context->structs, "{\n");
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

static void classMethodBegin(void* ud, const CompilerLocation* loc, const CompilerLocation* visLoc,
    CompilerVisibility vis, const CompilerLocation* optionalStatic, const CompilerLocation* retLoc, CompilerType* ret)
{
    Context* context = (Context*)ud;
    UNUSED(loc);
    UNUSED(visLoc);
    UNUSED(vis);
    UNUSED(optionalStatic);

    buffPrintC(&context->methods, '\n');
    printLine(context, &context->methods, retLoc);
    buffPrintF(&context->methods, "static ");
    printType(context, &context->methods, ret);
    buffPrintC(&context->methods, '\n');
}

static void classMethodNameSimple(void* ud, const CompilerLocation* loc, const char* name)
{
    Context* context = (Context*)ud;
    printLine(context, &context->methods, loc);
    buffPrintF(&context->methods, "%s_%s()\n", context->currentClass, name);
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

static void classMethodBodyBegin(void* ud)
{
    UNUSED(ud);
}

static void classMethodEnd(void* ud)
{
    UNUSED(ud);
}

static void classMembersEnd(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
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
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(optionalStatic);
    UNUSED(isConst);
}

static void varBegin(void* ud, const CompilerLocation* loc, const char* name)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
}

static void varType(void* ud, const CompilerLocation* loc, CompilerType* type)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(type);
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
    UNUSED(ud);
    UNUSED(value);
}

static void varEnd(void* ud)
{
    UNUSED(ud);
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

static CompilerType* typeBit(void* ud, const CompilerLocation* loc, CompilerExpr* optionalExpr)
{
    Context* context = (Context*)ud;
    UNUSED(loc);
    UNUSED(optionalExpr);
    return luaL_error(context->compiler->L, "bit type is not supported in bootstrap code generator."), NULL;
}

static CompilerType* typeBool(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return &g_bool;
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
    UNUSED(ud);
    UNUSED(loc);
    return NULL;
}

static CompilerExpr* exprFalse(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return NULL;
}

static CompilerExpr* exprTrue(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
    return NULL;
}

static CompilerExpr* exprIdentifier(void* ud, const CompilerLocation* loc, const char* name)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
    return NULL;
}

static CompilerExpr* exprInteger(void* ud, const CompilerLocation* loc, uint_value_t value)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(value);
    return NULL;
}

static CompilerExpr* exprParentheses(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(operand);
    return NULL;
}

static void exprNewBegin(void* ud, const CompilerLocation* loc, CompilerType* type)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(type);
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
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(arr);
    UNUSED(idx);
    return NULL;
}

static CompilerExpr* exprMember(void* ud,
    const CompilerLocation* loc, CompilerExpr* target, const CompilerLocation* nameLoc, const char* name)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(target);
    UNUSED(nameLoc);
    UNUSED(name);
    return NULL;
}

static CompilerExpr* exprPostfixIncr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(operand);
    return NULL;
}

static CompilerExpr* exprPostfixDecr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(operand);
    return NULL;
}

static CompilerExpr* exprPrefixIncr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(operand);
    return NULL;
}

static CompilerExpr* exprPrefixDecr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(operand);
    return NULL;
}

static CompilerExpr* exprTakeAddress(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(operand);
    return NULL;
}

static CompilerExpr* exprDeref(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(operand);
    return NULL;
}

static CompilerExpr* exprUnaryPlus(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(operand);
    return NULL;
}

static CompilerExpr* exprUnaryMinus(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(operand);
    return NULL;
}

static CompilerExpr* exprBitwiseNot(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(operand);
    return NULL;
}

static CompilerExpr* exprLogicNot(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(operand);
    return NULL;
}

static CompilerExpr* exprSizeOf(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(operand);
    return NULL;
}

static CompilerExpr* exprMultiply(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprDivide(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprModulo(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprAdd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprSubtract(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprShiftLeft(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprShiftRight(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprGreater(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprGreaterEq(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprLess(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprLessEq(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprEqual(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprNotEqual(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprBitwiseAnd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprBitwiseXor(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprBitwiseOr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprLogicAnd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprLogicOr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprTernary(void* ud, const CompilerLocation* loc,
    CompilerExpr* cond, CompilerExpr* trueValue, CompilerExpr* falseValue)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(cond);
    UNUSED(trueValue);
    UNUSED(falseValue);
    return NULL;
}

static CompilerExpr* exprAssign(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprAssignAdd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprAssignSub(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprAssignMul(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprAssignDiv(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprAssignMod(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprAssignAnd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprAssignOr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprAssignXor(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprAssignShl(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}

static CompilerExpr* exprAssignShr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(op1);
    UNUSED(op2);
    return NULL;
}


static void stmtEmpty(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtExpr(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(e);
}

static void stmtLabel(void* ud, const CompilerLocation* loc, const char* name)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
}

static void stmtGoto(void* ud, const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(nameLoc);
    UNUSED(name);
}

static void stmtBreak(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtContinue(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtDelete(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(e);
}

static void stmtThrow(void* ud, const CompilerLocation* loc, CompilerExpr* optionalExpr)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(optionalExpr);
}

static void stmtCompoundBegin(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    printLine(context, &context->methods, loc);
    buffPrintS(&context->methods, "{\n");
}

static void stmtCompoundEnd(void* ud, const CompilerLocation* loc)
{
    Context* context = (Context*)ud;
    printLine(context, &context->methods, loc);
    buffPrintS(&context->methods, "}\n");
}

static void stmtIfBegin(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtIfThen(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(e);
}

static void stmtIfElse(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtIfEnd(void* ud)
{
    UNUSED(ud);
}

static void stmtWhileBegin(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtWhileDo(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(e);
}

static void stmtWhileEnd(void* ud)
{
    UNUSED(ud);
}

static void stmtDoBegin(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtDoEnd(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(e);
}

static void stmtForBegin(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtForInit(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(e);
}

static void stmtForCondition(void* ud, CompilerExpr* e)
{
    UNUSED(ud);
    UNUSED(e);
}

static void stmtForUpdate(void* ud, CompilerExpr* e)
{
    UNUSED(ud);
    UNUSED(e);
}

static void stmtForDo(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtForEnd(void* ud)
{
    UNUSED(ud);
}

static void stmtSwitchBegin(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtSwitchOperand(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(e);
}

static void stmtSwitchCase(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(e);
}

static void stmtSwitchCaseRange(void* ud, const CompilerLocation* loc, CompilerExpr* start, CompilerExpr* end)
{
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(start);
    UNUSED(end);
}

static void stmtSwitchDefault(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtSwitchEnd(void* ud)
{
    UNUSED(ud);
}

static void stmtTryBegin(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtTry_CatchBegin(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtTry_CatchDo(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtTry_CatchEnd(void* ud)
{
    UNUSED(ud);
}

static void stmtTry_FinallyBegin(void* ud, const CompilerLocation* loc)
{
    UNUSED(ud);
    UNUSED(loc);
}

static void stmtTry_FinallyEnd(void* ud)
{
    UNUSED(ud);
}

static void stmtTryEnd(void* ud)
{
    UNUSED(ud);
}

static void error(void* ud, const CompilerLocation* loc, const CompilerToken* token)
{
    Context* context = (Context*)ud;
    UNUSED(loc);
    luaL_error(context->compiler->L, "unexpected token: %s\n", token->name);
}

/*==================================================================================================================*/

void compilerInitBootstrapCodegen(Compiler* compiler)
{
    Context* context = (Context*)compilerTempAlloc(compiler, sizeof(Context));
    context->compiler = compiler;
    context->file = compilerBeginOutput(compiler, NULL);
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
    compiler->parser.cb.classBeginInterface = classBeginInterface;
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
    compiler->parser.cb.typeInt8 = typeInt8;
    compiler->parser.cb.typeUInt8 = typeUInt8;
    compiler->parser.cb.typeInt16 = typeInt16;
    compiler->parser.cb.typeUInt16 = typeUInt16;
    compiler->parser.cb.typeInt32 = typeInt32;
    compiler->parser.cb.typeUInt32 = typeUInt32;
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
