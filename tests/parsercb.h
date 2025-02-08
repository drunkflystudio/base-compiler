
static int g_indent;

static void indent(void)
{
    ++g_indent;
}

static void unindent(void)
{
    assert(g_indent > 0);
    --g_indent;
}

static void frag(lua_State* L, int count)
{
    for (int i = 0; i < g_indent; i++)
        printC(' ');

    lua_concat(L, count);
    printS(lua_tostring(L, -1));
    printC('\n');
    lua_pop(L, 1);
}

#define FRAG(NAME) \
    { \
        lua_State* L = (lua_State*)ud; \
        const char* frag_name_ = #NAME; \
        int arg_n_ = 1; \
        lua_pushstring(L, frag_name_);

#define STR(NAME) \
        ++arg_n_; \
        if ((NAME) == NULL) \
            luaL_error(L, "%s: unexpected NULL for \"%s\".", frag_name_, #NAME); \
        lua_pushfstring(L, " %s:\"%s\"", #NAME, NAME);

#define OPTSTR(NAME) \
        ++arg_n_; \
        if (!NAME) \
            lua_pushfstring(L, " %s:%s", #NAME, "(null)"); \
        else \
            lua_pushfstring(L, " %s:\"%s\"", #NAME, NAME);

#define LOC(NAME) \
        ++arg_n_; \
        if ((NAME) == NULL) \
            luaL_error(L, "%s: unexpected NULL for \"%s\".", frag_name_, #NAME); \
        if ((NAME)->startLine == NULL) \
            luaL_error(L, "%s: %s is NULL in \"%s\".", "startLine", frag_name_, #NAME); \
        if ((NAME)->endLine == NULL) \
            luaL_error(L, "%s: %s is NULL in \"%s\".", "endLine", frag_name_, #NAME); \
        lua_pushfstring(L, " %s:(%d,%d-%d,%d)", #NAME, \
            NAME->startLine->number, \
            NAME->startColumn, \
            NAME->endLine->number, \
            NAME->endColumn);

#define OPTLOC(NAME) \
        ++arg_n_; \
        if (!NAME) \
            lua_pushfstring(L, " %s:%s", #NAME, "(null)"); \
        else { \
            if ((NAME)->startLine == NULL) \
                luaL_error(L, "%s: %s is NULL in \"%s\".", "startLine", frag_name_, #NAME); \
            if ((NAME)->endLine == NULL) \
                luaL_error(L, "%s: %s is NULL in \"%s\".", "endLine", frag_name_, #NAME); \
            lua_pushfstring(L, " %s:(%d,%d-%d,%d)", #NAME, \
                NAME->startLine->number, \
                NAME->startColumn, \
                NAME->endLine->number, \
                NAME->endColumn); \
        }

#define EXPR(NAME) \
        /* FIXME */ UNUSED(NAME);

#define END \
        frag(L, arg_n_); \
    }

static void translationUnitBegin(void* ud)
{
    FRAG(translationUnitBegin)
    END
    indent();
}

static void translationUnitEnd(void* ud)
{
    unindent();
    FRAG(translationUnitEnd)
    END
}

static void attrBegin(void* ud, const CompilerLocation* loc, const char* name)
{
    FRAG(attrBegin)
        LOC(loc)
        STR(name)
    END
    indent();
}

static void attrParam(void* ud, const CompilerLocation* optionalNameLoc, const char* optionalName, CompilerExpr* value)
{
    FRAG(attrParam)
        OPTLOC(optionalNameLoc)
        STR(optionalName)
        EXPR(value)
    END
}

static void attrEnd(void* ud)
{
    unindent();
    FRAG(attrEnd)
    END
}

static void attrListBegin(void* ud)
{
}

static void attrListEnd(void* ud)
{
}

static void classBegin(void* ud,
    const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name, bool isFinal)
{
}

static void classBeginInterface(void* ud,
    const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name)
{
}

static void classParent(void* ud, const CompilerLocation* loc, const char* name)
{
}

static void classMembersBegin(void* ud, const CompilerLocation* loc)
{
}

static void classFriend(void* ud, const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name)
{
}

static void classVariablesBegin(void* ud, const CompilerLocation* loc,
    const CompilerLocation* visLoc, CompilerVisibility vis, const CompilerLocation* optionalStatic, bool isConst)
{
}

static void classVariablesEnd(void* ud, const CompilerLocation* loc)
{
}

static void classDestructorBegin(void* ud,
    const CompilerLocation* loc, const char* name, const CompilerLocation* visLoc, CompilerVisibility vis)
{
}

static void classDestructorEnd(void* ud)
{
}

static void classMethodBegin(void* ud, const CompilerLocation* loc, const CompilerLocation* visLoc,
    CompilerVisibility vis, const CompilerLocation* optionalStatic, const CompilerLocation* retLoc, CompilerType* ret)
{
}

static void classMethodNameSimple(void* ud, const CompilerLocation* loc, const char* name)
{
}

static void classMethodNameArg(void* ud, const CompilerLocation* loc,
    const char* name, CompilerType* type, const CompilerLocation* argLoc, const char* arg)
{
}

static void classMethodNameEnd(void* ud,
    const CompilerLocation* optionalFinal, const CompilerLocation* optionalOverride)
{
}

static void classMethodEnd_Abstract(void* ud, const CompilerLocation* loc)
{
}

static void classMethodBodyBegin(void* ud)
{
}

static void classMethodEnd(void* ud)
{
}

static void classMembersEnd(void* ud, const CompilerLocation* loc)
{
}

static void varDeclBegin(void* ud, const CompilerLocation* loc, const CompilerLocation* optionalStatic, bool isConst)
{
}

static void varBegin(void* ud, const CompilerLocation* loc, const char* name)
{
}

static void varType(void* ud, const CompilerLocation* loc, CompilerType* type)
{
}

static void varType_Array(void* ud, const CompilerLocation* loc, CompilerType* elementType, CompilerExpr* size)
{
}

static void varInitializer(void* ud, CompilerExpr* value)
{
}

static void varEnd(void* ud)
{
}

static void varDeclEnd(void* ud, const CompilerLocation* loc)
{
}

static void structInitializerBegin(void* ud)
{
}

static void structInitializer(void* ud, const CompilerLocation* loc, const char* name, CompilerExpr* value)
{
}

static void structInitializerEnd(void* ud)
{
}

static void arrayInitializerBegin(void* ud, const CompilerLocation* loc)
{
}

static void arrayInitializer(void* ud, CompilerExpr* value)
{
}

static void arrayInitializerEnd(void* ud, const CompilerLocation* loc)
{
}

static CompilerType* typeVoid(void* ud, const CompilerLocation* loc)
{
    return NULL;
}

static CompilerType* typeBit(void* ud, const CompilerLocation* loc, CompilerExpr* expr)
{
    return NULL;
}

static CompilerType* typeBool(void* ud, const CompilerLocation* loc)
{
    return NULL;
}

static CompilerType* typeInt8(void* ud, const CompilerLocation* loc)
{
    return NULL;
}

static CompilerType* typeUInt8(void* ud, const CompilerLocation* loc)
{
    return NULL;
}

static CompilerType* typeInt16(void* ud, const CompilerLocation* loc)
{
    return NULL;
}

static CompilerType* typeUInt16(void* ud, const CompilerLocation* loc)
{
    return NULL;
}

static CompilerType* typeInt32(void* ud, const CompilerLocation* loc)
{
    return NULL;
}

static CompilerType* typeUInt32(void* ud, const CompilerLocation* loc)
{
    return NULL;
}

static CompilerType* typeObject(void* ud, const CompilerLocation* loc)
{
    return NULL;
}

static CompilerType* typeIdentifier(void* ud, const CompilerLocation* loc, const char* text)
{
    return NULL;
}

static CompilerType* typePointer(void* ud, const CompilerLocation* loc, CompilerType* pointee)
{
    return NULL;
}

static CompilerExpr* exprNull(void* ud, const CompilerLocation* loc)
{
    return NULL;
}

static CompilerExpr* exprFalse(void* ud, const CompilerLocation* loc)
{
    return NULL;
}

static CompilerExpr* exprTrue(void* ud, const CompilerLocation* loc)
{
    return NULL;
}

static CompilerExpr* exprIdentifier(void* ud, const CompilerLocation* loc, const char* name)
{
    return NULL;
}

static CompilerExpr* exprInteger(void* ud, const CompilerLocation* loc, uint_value_t value)
{
    return NULL;
}

static CompilerExpr* exprParentheses(void* ud, const CompilerLocation* loc, CompilerExpr* expr)
{
    return NULL;
}

static void exprNewBegin(void* ud, const CompilerLocation* loc, CompilerType* type)
{
}

static CompilerExpr* exprNewEnd_Struct(void* ud, const CompilerLocation* loc)
{
    return NULL;
}

static CompilerExpr* exprNewEnd_Array(void* ud, const CompilerLocation* loc, CompilerExpr* size)
{
    return NULL;
}

static void exprMethodCallBegin(void* ud, CompilerExpr* callee)
{
}

static void exprMethodSimple(void* ud, const CompilerLocation* loc, const char* name)
{
}

static void exprMethodArg(void* ud, const CompilerLocation* loc, const char* name, CompilerExpr* value)
{
}

static CompilerExpr* exprMethodCallEnd(void* ud, const CompilerLocation* loc)
{
    return NULL;
}

static CompilerExpr* exprSubscript(void* ud, const CompilerLocation* loc, CompilerExpr* arr, CompilerExpr* idx)
{
    return NULL;
}

static CompilerExpr* exprMember(void* ud,
    const CompilerLocation* loc, CompilerExpr* target, const CompilerLocation* nameLoc, const char* name)
{
    return NULL;
}

static CompilerExpr* exprPostfixIncr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    return NULL;
}

static CompilerExpr* exprPostfixDecr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    return NULL;
}

static CompilerExpr* exprPrefixIncr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    return NULL;
}

static CompilerExpr* exprPrefixDecr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    return NULL;
}

static CompilerExpr* exprTakeAddress(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    return NULL;
}

static CompilerExpr* exprDeref(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    return NULL;
}

static CompilerExpr* exprUnaryPlus(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    return NULL;
}

static CompilerExpr* exprUnaryMinus(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    return NULL;
}

static CompilerExpr* exprBitwiseNot(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    return NULL;
}

static CompilerExpr* exprLogicNot(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    return NULL;
}

static CompilerExpr* exprSizeOf(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    return NULL;
}

static CompilerExpr* exprMultiply(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprDivide(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprModulo(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprAdd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprSubtract(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprShiftLeft(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprShiftRight(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprGreater(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprGreaterEq(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprLess(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprLessEq(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprEqual(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprNotEqual(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprBitwiseAnd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprBitwiseXor(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprBitwiseOr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprLogicAnd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprLogicOr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprTernary(void* ud, const CompilerLocation* loc,
    CompilerExpr* cond, CompilerExpr* trueValue, CompilerExpr* falseValue)
{
    return NULL;
}

static CompilerExpr* exprAssign(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprAssignAdd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprAssignSub(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprAssignMul(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprAssignDiv(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprAssignMod(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprAssignAnd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprAssignOr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprAssignXor(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprAssignShl(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}

static CompilerExpr* exprAssignShr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    return NULL;
}


static void stmtEmpty(void* ud, const CompilerLocation* loc)
{
}

static void stmtExpr(void* ud, const CompilerLocation* loc, CompilerExpr* expr)
{
}

static void stmtLabel(void* ud, const CompilerLocation* loc, const char* name)
{
}

static void stmtGoto(void* ud, const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name)
{
}

static void stmtBreak(void* ud, const CompilerLocation* loc)
{
}

static void stmtContinue(void* ud, const CompilerLocation* loc)
{
}

static void stmtDelete(void* ud, const CompilerLocation* loc, CompilerExpr* expr)
{
}

static void stmtThrow(void* ud, const CompilerLocation* loc, CompilerExpr* optionalExpr)
{
}

static void stmtCompoundBegin(void* ud, const CompilerLocation* loc)
{
}

static void stmtCompoundEnd(void* ud, const CompilerLocation* loc)
{
}

static void stmtIfBegin(void* ud, const CompilerLocation* loc)
{
}

static void stmtIfThen(void* ud, const CompilerLocation* loc, CompilerExpr* expr)
{
}

static void stmtIfElse(void* ud, const CompilerLocation* loc)
{
}

static void stmtIfEnd(void* ud)
{
}

static void stmtWhileBegin(void* ud, const CompilerLocation* loc)
{
}

static void stmtWhileDo(void* ud, const CompilerLocation* loc, CompilerExpr* expr)
{
}

static void stmtWhileEnd(void* ud)
{
}

static void stmtDoBegin(void* ud, const CompilerLocation* loc)
{
}

static void stmtDoEnd(void* ud, const CompilerLocation* loc, CompilerExpr* expr)
{
}

static void stmtForBegin(void* ud, const CompilerLocation* loc)
{
}

static void stmtForInit(void* ud, const CompilerLocation* loc, CompilerExpr* expr)
{
}

static void stmtForCondition(void* ud, CompilerExpr* expr)
{
}

static void stmtForUpdate(void* ud, CompilerExpr* expr)
{
}

static void stmtForDo(void* ud, const CompilerLocation* loc)
{
}

static void stmtForEnd(void* ud)
{
}

static void stmtSwitchBegin(void* ud, const CompilerLocation* loc)
{
}

static void stmtSwitchOperand(void* ud, const CompilerLocation* loc, CompilerExpr* expr)
{
}

static void stmtSwitchCase(void* ud, const CompilerLocation* loc, CompilerExpr* expr)
{
}

static void stmtSwitchCaseRange(void* ud, const CompilerLocation* loc, CompilerExpr* start, CompilerExpr* end)
{
}

static void stmtSwitchDefault(void* ud, const CompilerLocation* loc)
{
}

static void stmtSwitchEnd(void* ud)
{
}

static void stmtTryBegin(void* ud, const CompilerLocation* loc)
{
}

static void stmtTry_CatchBegin(void* ud, const CompilerLocation* loc)
{
}

static void stmtTry_CatchDo(void* ud, const CompilerLocation* loc)
{
}

static void stmtTry_CatchEnd(void* ud)
{
}

static void stmtTry_FinallyBegin(void* ud, const CompilerLocation* loc)
{
}

static void stmtTry_FinallyEnd(void* ud)
{
}

static void stmtTryEnd(void* ud)
{
}

static void initParserCallbacks(CompilerParser* parser)
{
    parser->cb.translationUnitBegin = translationUnitBegin;
    parser->cb.translationUnitEnd = translationUnitEnd;
    parser->cb.attrBegin = attrBegin;
    parser->cb.attrParam = attrParam;
    parser->cb.attrEnd = attrEnd;
    parser->cb.attrListBegin = attrListBegin;
    parser->cb.attrListEnd = attrListEnd;
    parser->cb.classBegin = classBegin;
    parser->cb.classBeginInterface = classBeginInterface;
    parser->cb.classParent = classParent;
    parser->cb.classMembersBegin = classMembersBegin;
    parser->cb.classFriend = classFriend;
    parser->cb.classVariablesBegin = classVariablesBegin;
    parser->cb.classVariablesEnd = classVariablesEnd;
    parser->cb.classDestructorBegin = classDestructorBegin;
    parser->cb.classDestructorEnd = classDestructorEnd;
    parser->cb.classMethodBegin = classMethodBegin;
    parser->cb.classMethodNameSimple = classMethodNameSimple;
    parser->cb.classMethodNameArg = classMethodNameArg;
    parser->cb.classMethodNameEnd = classMethodNameEnd;
    parser->cb.classMethodEnd_Abstract = classMethodEnd_Abstract;
    parser->cb.classMethodBodyBegin = classMethodBodyBegin;
    parser->cb.classMethodEnd = classMethodEnd;
    parser->cb.classMembersEnd = classMembersEnd;
    parser->cb.varDeclBegin = varDeclBegin;
    parser->cb.varBegin = varBegin;
    parser->cb.varType = varType;
    parser->cb.varType_Array = varType_Array;
    parser->cb.varInitializer = varInitializer;
    parser->cb.varEnd = varEnd;
    parser->cb.varDeclEnd = varDeclEnd;
    parser->cb.structInitializerBegin = structInitializerBegin;
    parser->cb.structInitializer = structInitializer;
    parser->cb.structInitializerEnd = structInitializerEnd;
    parser->cb.arrayInitializerBegin = arrayInitializerBegin;
    parser->cb.arrayInitializer = arrayInitializer;
    parser->cb.arrayInitializerEnd = arrayInitializerEnd;
    parser->cb.typeVoid = typeVoid;
    parser->cb.typeBit = typeBit;
    parser->cb.typeBool = typeBool;
    parser->cb.typeInt8 = typeInt8;
    parser->cb.typeUInt8 = typeUInt8;
    parser->cb.typeInt16 = typeInt16;
    parser->cb.typeUInt16 = typeUInt16;
    parser->cb.typeInt32 = typeInt32;
    parser->cb.typeUInt32 = typeUInt32;
    parser->cb.typeObject = typeObject;
    parser->cb.typeIdentifier = typeIdentifier;
    parser->cb.typePointer = typePointer;
    parser->cb.exprNull = exprNull;
    parser->cb.exprFalse = exprFalse;
    parser->cb.exprTrue = exprTrue;
    parser->cb.exprIdentifier = exprIdentifier;
    parser->cb.exprInteger = exprInteger;
    parser->cb.exprParentheses = exprParentheses;
    parser->cb.exprNewBegin = exprNewBegin;
    parser->cb.exprNewEnd_Struct = exprNewEnd_Struct;
    parser->cb.exprNewEnd_Array = exprNewEnd_Array;
    parser->cb.exprMethodCallBegin = exprMethodCallBegin;
    parser->cb.exprMethodSimple = exprMethodSimple;
    parser->cb.exprMethodArg = exprMethodArg;
    parser->cb.exprMethodCallEnd = exprMethodCallEnd;
    parser->cb.exprSubscript = exprSubscript;
    parser->cb.exprMember = exprMember;
    parser->cb.exprPostfixIncr = exprPostfixIncr;
    parser->cb.exprPostfixDecr = exprPostfixDecr;
    parser->cb.exprPrefixIncr = exprPrefixIncr;
    parser->cb.exprPrefixDecr = exprPrefixDecr;
    parser->cb.exprTakeAddress = exprTakeAddress;
    parser->cb.exprDeref = exprDeref;
    parser->cb.exprUnaryPlus = exprUnaryPlus;
    parser->cb.exprUnaryMinus = exprUnaryMinus;
    parser->cb.exprBitwiseNot = exprBitwiseNot;
    parser->cb.exprLogicNot = exprLogicNot;
    parser->cb.exprSizeOf = exprSizeOf;
    parser->cb.exprMultiply = exprMultiply;
    parser->cb.exprDivide = exprDivide;
    parser->cb.exprModulo = exprModulo;
    parser->cb.exprAdd = exprAdd;
    parser->cb.exprSubtract = exprSubtract;
    parser->cb.exprShiftLeft = exprShiftLeft;
    parser->cb.exprShiftRight = exprShiftRight;
    parser->cb.exprGreater = exprGreater;
    parser->cb.exprGreaterEq = exprGreaterEq;
    parser->cb.exprLess = exprLess;
    parser->cb.exprLessEq = exprLessEq;
    parser->cb.exprEqual = exprEqual;
    parser->cb.exprNotEqual = exprNotEqual;
    parser->cb.exprBitwiseAnd = exprBitwiseAnd;
    parser->cb.exprBitwiseXor = exprBitwiseXor;
    parser->cb.exprBitwiseOr = exprBitwiseOr;
    parser->cb.exprLogicAnd = exprLogicAnd;
    parser->cb.exprLogicOr = exprLogicOr;
    parser->cb.exprTernary = exprTernary;
    parser->cb.exprAssign = exprAssign;
    parser->cb.exprAssignAdd = exprAssignAdd;
    parser->cb.exprAssignSub = exprAssignSub;
    parser->cb.exprAssignMul = exprAssignMul;
    parser->cb.exprAssignDiv = exprAssignDiv;
    parser->cb.exprAssignMod = exprAssignMod;
    parser->cb.exprAssignAnd = exprAssignAnd;
    parser->cb.exprAssignOr = exprAssignOr;
    parser->cb.exprAssignXor = exprAssignXor;
    parser->cb.exprAssignShl = exprAssignShl;
    parser->cb.exprAssignShr = exprAssignShr;
    parser->cb.stmtEmpty = stmtEmpty;
    parser->cb.stmtExpr = stmtExpr;
    parser->cb.stmtLabel = stmtLabel;
    parser->cb.stmtGoto = stmtGoto;
    parser->cb.stmtBreak = stmtBreak;
    parser->cb.stmtContinue = stmtContinue;
    parser->cb.stmtDelete = stmtDelete;
    parser->cb.stmtThrow = stmtThrow;
    parser->cb.stmtCompoundBegin = stmtCompoundBegin;
    parser->cb.stmtCompoundEnd = stmtCompoundEnd;
    parser->cb.stmtIfBegin = stmtIfBegin;
    parser->cb.stmtIfThen = stmtIfThen;
    parser->cb.stmtIfElse = stmtIfElse;
    parser->cb.stmtIfEnd = stmtIfEnd;
    parser->cb.stmtWhileBegin = stmtWhileBegin;
    parser->cb.stmtWhileDo = stmtWhileDo;
    parser->cb.stmtWhileEnd = stmtWhileEnd;
    parser->cb.stmtDoBegin = stmtDoBegin;
    parser->cb.stmtDoEnd = stmtDoEnd;
    parser->cb.stmtForBegin = stmtForBegin;
    parser->cb.stmtForInit = stmtForInit;
    parser->cb.stmtForCondition = stmtForCondition;
    parser->cb.stmtForUpdate = stmtForUpdate;
    parser->cb.stmtForDo = stmtForDo;
    parser->cb.stmtForEnd = stmtForEnd;
    parser->cb.stmtSwitchBegin = stmtSwitchBegin;
    parser->cb.stmtSwitchOperand = stmtSwitchOperand;
    parser->cb.stmtSwitchCase = stmtSwitchCase;
    parser->cb.stmtSwitchCaseRange = stmtSwitchCaseRange;
    parser->cb.stmtSwitchDefault = stmtSwitchDefault;
    parser->cb.stmtSwitchEnd = stmtSwitchEnd;
    parser->cb.stmtTryBegin = stmtTryBegin;
    parser->cb.stmtTry_CatchBegin = stmtTry_CatchBegin;
    parser->cb.stmtTry_CatchDo = stmtTry_CatchDo;
    parser->cb.stmtTry_CatchEnd = stmtTry_CatchEnd;
    parser->cb.stmtTry_FinallyBegin = stmtTry_FinallyBegin;
    parser->cb.stmtTry_FinallyEnd = stmtTry_FinallyEnd;
    parser->cb.stmtTryEnd = stmtTryEnd;
}
