#ifndef DRUNKFLY_COMPILER_PARSER_H
#define DRUNKFLY_COMPILER_PARSER_H

#include <drunkfly/compiler.h>
#include <drunkfly/compiler/token.h>

#define VIS(NAME) CompilerVisibility NAME
#define LOC(NAME) const CompilerLocation* NAME
#define EXPR(NAME) CompilerExpr* NAME
#define TYPE(NAME) CompilerType* NAME

STRUCT(CompilerParserCallbacks)
{
    void* ud;

    /* translation unit */
    void (*translationUnitBegin)(void* ud);
    void (*translationUnitEnd)(void* ud);

    /* attributes */
    void (*attrBegin)(void* ud, LOC(loc), const char* name);
    void (*attrParam)(void* ud, LOC(optionalNameLoc), const char* optionalName, EXPR(value));
    void (*attrEnd)(void* ud);
    void (*attrListBegin)(void* ud);
    void (*attrListEnd)(void* ud);

    /* enum declaration */
    void (*enumBegin)(void* ud, LOC(loc), LOC(nameLoc), const char* name, bool isFlags);
    void (*enumType)(void* ud, LOC(loc), TYPE(type));
    void (*enumMembersBegin)(void* ud, LOC(loc));
    void (*enumMember)(void* ud, LOC(loc), const char* name, EXPR(optionalValue));
    void (*enumMembersEnd)(void* ud, LOC(loc));
    void (*enumEnd)(void* ud, LOC(loc));

    /* struct declaration */
    void (*structBegin)(void* ud, LOC(loc), LOC(nameLoc), const char* name, bool isUnion);
    void (*structParent)(void* ud, LOC(loc), const char* name);
    void (*structMembersBegin)(void* ud, LOC(loc));
    void (*structMember)(void* ud, LOC(loc), const char* name, TYPE(type));
    void (*structMembersEnd)(void* ud, LOC(loc));
    void (*structEnd)(void* ud, LOC(loc));

    /* class/interface declaration */
    void (*classBegin)(void* ud, LOC(loc), LOC(nameLoc), const char* name, bool isFinal);
    void (*classBeginInterface)(void* ud, LOC(loc), LOC(nameLoc), const char* name);
    void (*classParent)(void* ud, LOC(loc), const char* name);
    void (*classMembersBegin)(void* ud, LOC(loc));
    void (*classFriend)(void* ud, LOC(loc), LOC(nameLoc), const char* name);
    void (*classVariablesBegin)(void* ud, LOC(loc), LOC(visLoc), VIS(vis), LOC(optionalStatic), bool isConst);
    void (*classVariablesEnd)(void* ud, LOC(loc));
    void (*classDestructorBegin)(void* ud, LOC(loc), const char* name, LOC(visLoc), VIS(vis));
    void (*classDestructorEnd)(void* ud);
    void (*classMethodBegin)(void* ud, LOC(loc), LOC(visLoc), VIS(vis), LOC(optionalStatic), LOC(retLoc), TYPE(ret));
    void (*classMethodNameSimple)(void* ud, LOC(loc), const char* name);
    void (*classMethodNameArg)(void* ud, LOC(loc), const char* name, TYPE(type), LOC(argLoc), const char* arg);
    void (*classMethodNameEnd)(void* ud, LOC(optionalFinal), LOC(optionalOverride));
    void (*classMethodEnd_Abstract)(void* ud, LOC(loc));
    void (*classMethodEnd_Extern)(void* ud, LOC(loc));
    void (*classMethodBodyBegin)(void* ud);
    void (*classMethodEnd)(void* ud);
    void (*classMembersEnd)(void* ud, LOC(loc));
    void (*classEnd)(void* ud, LOC(loc));

    /* variable declaration */
    void (*varDeclBegin)(void* ud, LOC(loc), LOC(optionalStatic), bool isConst);
    void (*varBegin)(void* ud, LOC(loc), const char* name);
    void (*varType)(void* ud, LOC(loc), TYPE(type));
    void (*varType_Array)(void* ud, LOC(loc), TYPE(elementType), EXPR(size));
    void (*varInitializer)(void* ud, EXPR(value));
    void (*varEnd)(void* ud);
    void (*varDeclEnd)(void* ud, LOC(loc));

    /* struct initializer */
    void (*structInitializerBegin)(void* ud);
    void (*structInitializer)(void* ud, LOC(loc), const char* name, EXPR(value));
    void (*structInitializerEnd)(void* ud);

    /* array initializer */
    void (*arrayInitializerBegin)(void* ud, LOC(loc));
    void (*arrayInitializer)(void* ud, EXPR(value));
    void (*arrayInitializerEnd)(void* ud, LOC(loc));

    /* type */
    CompilerType* (*typeVoid)(void* ud, LOC(loc));
    CompilerType* (*typeBit)(void* ud, LOC(loc), EXPR(optionalExpr));
    CompilerType* (*typeBool)(void* ud, LOC(loc));
    CompilerType* (*typeInt8)(void* ud, LOC(loc));
    CompilerType* (*typeUInt8)(void* ud, LOC(loc));
    CompilerType* (*typeInt16)(void* ud, LOC(loc));
    CompilerType* (*typeUInt16)(void* ud, LOC(loc));
    CompilerType* (*typeInt32)(void* ud, LOC(loc));
    CompilerType* (*typeUInt32)(void* ud, LOC(loc));
    CompilerType* (*typeObject)(void* ud, LOC(loc));
    CompilerType* (*typeIdentifier)(void* ud, LOC(loc), const char* text);
    CompilerType* (*typePointer)(void* ud, LOC(loc), TYPE(pointee));

    /* expression */
    CompilerExpr* (*exprNull)(void* ud, LOC(loc));
    CompilerExpr* (*exprFalse)(void* ud, LOC(loc));
    CompilerExpr* (*exprTrue)(void* ud, LOC(loc));
    CompilerExpr* (*exprIdentifier)(void* ud, LOC(loc), const char* name);
    CompilerExpr* (*exprInteger)(void* ud, LOC(loc), uint_value_t value);
    CompilerExpr* (*exprParentheses)(void* ud, LOC(loc), EXPR(operand));
    void          (*exprNewBegin)(void* ud, LOC(loc), TYPE(type));
    CompilerExpr* (*exprNewEnd_Struct)(void* ud, LOC(loc));
    CompilerExpr* (*exprNewEnd_Array)(void* ud, LOC(loc), EXPR(size));
    void          (*exprMethodCallBegin)(void* ud, EXPR(callee));
    void          (*exprMethodSimple)(void* ud, LOC(loc), const char* name);
    void          (*exprMethodArg)(void* ud, LOC(loc), const char* name, EXPR(value));
    CompilerExpr* (*exprMethodCallEnd)(void* ud, LOC(loc));
    CompilerExpr* (*exprSubscript)(void* ud, LOC(loc), EXPR(arr), EXPR(idx));
    CompilerExpr* (*exprMember)(void* ud, LOC(loc), EXPR(target), LOC(nameLoc), const char* name);
    CompilerExpr* (*exprPostfixIncr)(void* ud, LOC(loc), EXPR(operand));
    CompilerExpr* (*exprPostfixDecr)(void* ud, LOC(loc), EXPR(operand));
    CompilerExpr* (*exprPrefixIncr)(void* ud, LOC(loc), EXPR(operand));
    CompilerExpr* (*exprPrefixDecr)(void* ud, LOC(loc), EXPR(operand));
    CompilerExpr* (*exprTakeAddress)(void* ud, LOC(loc), EXPR(operand));
    CompilerExpr* (*exprDeref)(void* ud, LOC(loc), EXPR(operand));
    CompilerExpr* (*exprUnaryPlus)(void* ud, LOC(loc), EXPR(operand));
    CompilerExpr* (*exprUnaryMinus)(void* ud, LOC(loc), EXPR(operand));
    CompilerExpr* (*exprBitwiseNot)(void* ud, LOC(loc), EXPR(operand));
    CompilerExpr* (*exprLogicNot)(void* ud, LOC(loc), EXPR(operand));
    CompilerExpr* (*exprSizeOf)(void* ud, LOC(loc), EXPR(operand));
    CompilerExpr* (*exprMultiply)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprDivide)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprModulo)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprAdd)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprSubtract)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprShiftLeft)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprShiftRight)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprGreater)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprGreaterEq)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprLess)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprLessEq)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprEqual)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprNotEqual)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprBitwiseAnd)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprBitwiseXor)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprBitwiseOr)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprLogicAnd)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprLogicOr)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprTernary)(void* ud, LOC(loc), EXPR(cond), EXPR(trueValue), EXPR(falseValue));
    CompilerExpr* (*exprAssign)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprAssignAdd)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprAssignSub)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprAssignMul)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprAssignDiv)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprAssignMod)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprAssignAnd)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprAssignOr)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprAssignXor)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprAssignShl)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));
    CompilerExpr* (*exprAssignShr)(void* ud, LOC(loc), EXPR(op1), EXPR(op2));

    /* statement */
    void (*stmtEmpty)(void* ud, LOC(loc));
    void (*stmtExpr)(void* ud, LOC(loc), EXPR(expr));
    void (*stmtLabel)(void* ud, LOC(loc), const char* name);
    void (*stmtGoto)(void* ud, LOC(loc), LOC(nameLoc), const char* name);
    void (*stmtBreak)(void* ud, LOC(loc));
    void (*stmtContinue)(void* ud, LOC(loc));
    void (*stmtDelete)(void* ud, LOC(loc), EXPR(expr));
    void (*stmtThrow)(void* ud, LOC(loc), EXPR(optionalExpr));
    void (*stmtReturn)(void* ud, LOC(loc), EXPR(optionalExpr));
    void (*stmtCompoundBegin)(void* ud, LOC(loc));
    void (*stmtCompoundEnd)(void* ud, LOC(loc));
    void (*stmtIfBegin)(void* ud, LOC(loc));
    void (*stmtIfThen)(void* ud, LOC(loc), EXPR(expr));
    void (*stmtIfElse)(void* ud, LOC(loc));
    void (*stmtIfEnd)(void* ud);
    void (*stmtWhileBegin)(void* ud, LOC(loc));
    void (*stmtWhileDo)(void* ud, LOC(loc), EXPR(expr));
    void (*stmtWhileEnd)(void* ud);
    void (*stmtDoBegin)(void* ud, LOC(loc));
    void (*stmtDoEnd)(void* ud, LOC(loc), EXPR(expr));
    void (*stmtForBegin)(void* ud, LOC(loc));
    void (*stmtForInit)(void* ud, LOC(loc), EXPR(expr));
    void (*stmtForCondition)(void* ud, EXPR(expr));
    void (*stmtForUpdate)(void* ud, EXPR(expr));
    void (*stmtForDo)(void* ud, LOC(loc));
    void (*stmtForEnd)(void* ud);
    void (*stmtSwitchBegin)(void* ud, LOC(loc));
    void (*stmtSwitchOperand)(void* ud, LOC(loc), EXPR(expr));
    void (*stmtSwitchCase)(void* ud, LOC(loc), EXPR(expr));
    void (*stmtSwitchCaseRange)(void* ud, LOC(loc), EXPR(start), EXPR(end));
    void (*stmtSwitchDefault)(void* ud, LOC(loc));
    void (*stmtSwitchEnd)(void* ud);
    void (*stmtTryBegin)(void* ud, LOC(loc));
    void (*stmtTry_CatchBegin)(void* ud, LOC(loc));
    void (*stmtTry_CatchDo)(void* ud, LOC(loc));
    void (*stmtTry_CatchEnd)(void* ud);
    void (*stmtTry_FinallyBegin)(void* ud, LOC(loc));
    void (*stmtTry_FinallyEnd)(void* ud);
    void (*stmtTryEnd)(void* ud);

    /* error */
    void (*error)(void* ud, LOC(loc), const CompilerToken* token);
};

#undef TYPE
#undef EXPR
#undef LOC
#undef VIS

STRUCT(CompilerParser)
{
    Compiler* compiler;
    CompilerParserCallbacks cb;
};

void compilerBeginParse(Compiler* compiler);
void compilerPushToken(Compiler* compiler);
void compilerPushTokenEx(Compiler* compiler, const CompilerToken* token);
void compilerPushTokenEof(Compiler* compiler, SourceLine* line, int column);
void compilerEndParse(Compiler* compiler);

#endif
