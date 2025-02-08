
struct CompilerType
{
    const char* name;
    CompilerType* pointee;
    CompilerExpr* size;
};

static CompilerType g_void = { "void" };
static CompilerType g_bit = { "bit" };
static CompilerType g_bool = { "bool" };
static CompilerType g_int8 = { "int8" };
static CompilerType g_uint8 = { "uint8" };
static CompilerType g_int16 = { "int16" };
static CompilerType g_uint16 = { "uint16" };
static CompilerType g_int32 = { "int32" };
static CompilerType g_uint32 = { "uint32" };
static CompilerType g_object = { "object" };

/*==================================================================================================================*/

typedef enum EnumID {
    EXPR_NULL,
    EXPR_FALSE,
    EXPR_TRUE,
    EXPR_IDENTIFIER,
    EXPR_INTEGER,
} EnumID;

struct CompilerExpr
{
    EnumID what;
    CompilerLocation loc;
    const char* identifier;
    uint_value_t integer;
};

/*==================================================================================================================*/

static int g_indent;

static void indent(void)
{
    ++g_indent;
}

static void unindent(void)
{
    if (g_indent <= 0) {
        logPrintf("%s: TEST FAILURE: invalid unindent().\n", g_testName);
        ++g_testFailCount;
        return;
    }
    --g_indent;
}

#ifndef NDEBUG

static bool isBegin(const char* str)
{
    size_t len = strlen(str);
    return len > 5 && !strcmp(str + len - 5, "Begin");
}

static bool isEnd(const char* str)
{
    size_t len = strlen(str);
    return len > 3 && !strcmp(str + len - 3, "End");
}

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define ASSERT_NAME_IS(NAME) assert(!strcmp(__func__, NAME))
#elif defined(__MSC_VER) || (defined(__GNUC__) && __GNUC__ >= 2)
#define ASSERT_NAME_IS(NAME) assert(!strcmp(__FUNCTION__, NAME))
#else
#define ASSERT_NAME_IS(NAME) ((void)0)
#endif

#endif /* NDEBUG */

/*==================================================================================================================*/

#define NOT_NULL(NAME) \
    if ((NAME) == NULL) { \
        logPrintf("%s: TEST FAILURE: %s: unexpected NULL for \"%s\".\n", g_testName, frag_name_, #NAME); \
        ++g_testFailCount; \
    }

#define FIELD_NOT_NULL(NAME, FIELD) \
    if ((NAME)->FIELD == NULL) { \
        logPrintf("%s: TEST FAILURE: %s: %s is NULL in \"%s\".\n", g_testName, frag_name_, #FIELD, #NAME); \
        ++g_testFailCount; \
    }

/*==================================================================================================================*/

static void printExpr(lua_State* L, CompilerExpr* expr)
{
    switch (expr->what) {
        case EXPR_NULL: lua_pushliteral(L, "null"); return;
        case EXPR_FALSE: lua_pushliteral(L, "false"); return;
        case EXPR_TRUE: lua_pushliteral(L, "true"); return;
        case EXPR_IDENTIFIER: lua_pushstring(L, expr->identifier); return;
        case EXPR_INTEGER: pushHex(L, expr->integer); return;
    }

    logPrintf("%s: TEST FAILURE: invalid expression.\n", g_testName);
    ++g_testFailCount;

    lua_pushliteral(L, "?");
}

/*==================================================================================================================*/

static void frag(lua_State* L, int count)
{
    int i;

    for (i = 0; i < g_indent; i++)
        printC(' ');

    lua_concat(L, count);
    printS(lua_tostring(L, -1));
    printC('\n');
    lua_pop(L, 1);
}

#define FRAG(NAME) \
    { \
        ParserTestContext* context = (ParserTestContext*)ud; \
        lua_State* L = context->L; \
        const char* frag_name_ = #NAME; \
        int arg_n_ = 1; \
        lua_pushstring(L, frag_name_); \
        ASSERT_NAME_IS(#NAME);

#define FRAG_UNINDENT(NAME) \
    assert(isEnd(#NAME)); \
    unindent(); \
    FRAG(NAME)

#define BOOL(NAME) \
        ++arg_n_; \
        lua_pushfstring(L, " %s:%s", #NAME, ((NAME) ? "true" : "false"));

#define VIS(NAME) \
        switch (NAME) { \
            case COMPILER_PRIVATE: ++arg_n_; lua_pushfstring(L, " %s:%s", #NAME, "private"); break; \
            case COMPILER_PROTECTED: ++arg_n_; lua_pushfstring(L, " %s:%s", #NAME, "protected"); break; \
            case COMPILER_PUBLIC: ++arg_n_; lua_pushfstring(L, " %s:%s", #NAME, "public"); break; \
            default: \
                logPrintf("%s: TEST FAILURE: %s: invalid value for \"%s\".\n", g_testName, frag_name_, #NAME); \
                ++g_testFailCount; \
        }

#define INT(NAME) \
        ++arg_n_; \
        lua_pushfstring(L, " %s:%s", #NAME, pushHex(L, NAME)); \
        lua_remove(L, -2); /* remove value pushed by pushHex() */

#define OPTSTR(NAME) \
        ++arg_n_; \
        if (!NAME) \
            lua_pushfstring(L, " %s:%s", #NAME, "(null)"); \
        else \
            lua_pushfstring(L, " %s:\"%s\"", #NAME, NAME);

#define STR(NAME) \
        NOT_NULL(NAME) \
        OPTSTR(NAME)

#define OPTLOC(NAME) \
        ++arg_n_; \
        if (!NAME) \
            lua_pushfstring(L, " %s:%s", #NAME, "(null)"); \
        else { \
            FIELD_NOT_NULL(NAME, startLine) \
            FIELD_NOT_NULL(NAME, endLine) \
            lua_pushfstring(L, " %s:(%d,%d-%d,%d)", #NAME, \
                NAME->startLine ? NAME->startLine->number : 0, \
                NAME->startColumn, \
                NAME->endLine ? NAME->endLine->number : 0, \
                NAME->endColumn); \
        }

#define LOC(NAME) \
        NOT_NULL(NAME) \
        OPTLOC(NAME)

#define OPTTYPE(NAME) \
        ++arg_n_; \
        if (!NAME) \
            lua_pushfstring(L, " %s:%s", #NAME, "(null)"); \
        else { \
            CompilerType* t = NAME; \
            while (t->pointee) \
                t = t->pointee; \
            lua_pushfstring(L, " %s:", #NAME); \
            if (t->name) { \
                ++arg_n_; \
                lua_pushstring(L, t->name); \
            } \
            if (t->size) { \
                int n = lua_gettop(L); \
                lua_pushliteral(L, "["); \
                printExpr(L, t->size); \
                lua_pushliteral(L, "]"); \
                arg_n_ += lua_gettop(L) - n; \
            } \
            t = NAME; \
            for (; t->pointee; t = t->pointee) { \
                ++arg_n_; \
                lua_pushliteral(L, "*"); \
            } \
        }

#define TYPE(NAME) \
        NOT_NULL(NAME) \
        OPTTYPE(NAME)

#define OPTEXPR(NAME) \
        if (!NAME) { \
            ++arg_n_; \
            lua_pushfstring(L, " %s:%s", #NAME, "(null)"); \
        } else { \
            int n = lua_gettop(L); \
            lua_pushfstring(L, " %s:", #NAME); \
            printExpr(L, NAME); \
            arg_n_ += lua_gettop(L) - n; \
        }

#define EXPR(NAME) \
        NOT_NULL(NAME) \
        OPTEXPR(NAME)

#define END \
        frag(L, arg_n_); \
    }

#define END_INDENT \
        assert(isBegin(frag_name_)); \
    END \
    indent();

/*==================================================================================================================*/

static void translationUnitBegin(void* ud)
{
    if (g_parseMode != PARSE_GLOBAL)
        return;

    FRAG(translationUnitBegin)
    END_INDENT
}

static void translationUnitEnd(void* ud)
{
    if (g_parseMode != PARSE_GLOBAL)
        return;

    FRAG_UNINDENT(translationUnitEnd)
    END
}

static void attrBegin(void* ud, const CompilerLocation* loc, const char* name)
{
    FRAG(attrBegin)
        LOC(loc)
        STR(name)
    END_INDENT
}

static void attrParam(void* ud, const CompilerLocation* optionalNameLoc, const char* optionalName, CompilerExpr* value)
{
    FRAG(attrParam)
        OPTLOC(optionalNameLoc)
        OPTSTR(optionalName)
        EXPR(value)
    END
}

static void attrEnd(void* ud)
{
    FRAG_UNINDENT(attrEnd)
    END
}

static void attrListBegin(void* ud)
{
    FRAG(attrListBegin)
    END_INDENT
}

static void attrListEnd(void* ud)
{
    FRAG_UNINDENT(attrListEnd)
    END
}

static void enumBegin(void* ud,
    const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name, bool isFlags)
{
    if (g_parseMode == PARSE_ATTR) {
        FRAG(enum)
            LOC(nameLoc)
        END
    } else {
        FRAG(enumBegin)
            LOC(loc)
            LOC(nameLoc)
            STR(name)
            BOOL(isFlags)
        END_INDENT
    }
}

static void enumType(void* ud, const CompilerLocation* loc, CompilerType* type)
{
    FRAG(enumType)
        LOC(loc)
        TYPE(type)
    END
}

static void enumMembersBegin(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode != PARSE_ATTR) {
        FRAG(enumMembersBegin)
            LOC(loc)
        END_INDENT
    }
}

static void enumMember(void* ud, const CompilerLocation* loc, const char* name, CompilerExpr* optionalValue)
{
    FRAG(enumMember)
        LOC(loc)
        STR(name)
        OPTEXPR(optionalValue)
    END
}

static void enumMembersEnd(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode != PARSE_ATTR) {
        FRAG_UNINDENT(enumMembersEnd)
            LOC(loc)
        END
    }
}

static void enumEnd(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode != PARSE_ATTR) {
        FRAG_UNINDENT(enumEnd)
            LOC(loc)
        END
    }
}

static void structBegin(void* ud,
    const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name, bool isUnion)
{
    if (g_parseMode == PARSE_ATTR) {
        FRAG(struct)
            LOC(nameLoc)
        END_INDENT
    } else {
        FRAG(structBegin)
            LOC(loc)
            LOC(nameLoc)
            STR(name)
            BOOL(isUnion)
        END_INDENT
    }
}

static void structParent(void* ud, const CompilerLocation* loc, const char* name)
{
    FRAG(enumType)
        LOC(loc)
        STR(name)
    END
}

static void structMembersBegin(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode != PARSE_ATTR) {
        FRAG(structMembersBegin)
            LOC(loc)
        END_INDENT
    }
}

static void structMember(void* ud, const CompilerLocation* loc, const char* name, CompilerType* type)
{
    FRAG(structMember)
        LOC(loc)
        STR(name)
        TYPE(type)
    END
}

static void structMembersEnd(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode != PARSE_ATTR) {
        FRAG_UNINDENT(structMembersEnd)
            LOC(loc)
        END
    }
}

static void structEnd(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode != PARSE_ATTR) {
        FRAG_UNINDENT(structEnd)
            LOC(loc)
        END
    }
}

static void classBegin(void* ud,
    const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name, bool isFinal)
{
    if (g_parseMode != PARSE_ATTR && g_parseMode != PARSE_TYPES && g_parseMode != PARSE_EXPR) {
        FRAG(classBegin)
            LOC(loc)
            LOC(nameLoc)
            STR(name)
            BOOL(isFinal)
        END_INDENT
    }
}

static void classBeginInterface(void* ud,
    const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name)
{
    if (g_parseMode != PARSE_ATTR) {
        FRAG(classBeginInterface)
            LOC(loc)
            LOC(nameLoc)
            STR(name)
        END_INDENT
    }
}

static void classParent(void* ud, const CompilerLocation* loc, const char* name)
{
    FRAG(classParent)
        LOC(loc)
        STR(name)
    END
}

static void classMembersBegin(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode != PARSE_ATTR && g_parseMode != PARSE_TYPES && g_parseMode != PARSE_EXPR) {
        FRAG(classMembersBegin)
            LOC(loc)
        END_INDENT
    }
}

static void classFriend(void* ud, const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name)
{
    FRAG(classFriend)
        LOC(loc)
        LOC(nameLoc)
        STR(name)
    END
}

static void classVariablesBegin(void* ud, const CompilerLocation* loc,
    const CompilerLocation* visLoc, CompilerVisibility vis, const CompilerLocation* optionalStatic, bool isConst)
{
    if (g_parseMode != PARSE_TYPES && g_parseMode != PARSE_EXPR) {
        FRAG(classVariablesBegin)
            LOC(loc)
            LOC(visLoc)
            VIS(vis)
            OPTLOC(optionalStatic)
            BOOL(isConst)
        END_INDENT
    }
}

static void classVariablesEnd(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode != PARSE_TYPES && g_parseMode != PARSE_EXPR) {
        FRAG_UNINDENT(classVariablesEnd)
            LOC(loc)
        END
    }
}

static void classDestructorBegin(void* ud,
    const CompilerLocation* loc, const char* name, const CompilerLocation* visLoc, CompilerVisibility vis)
{
    FRAG(classDestructorBegin)
        LOC(loc)
        STR(name)
        LOC(visLoc)
        VIS(vis)
    END_INDENT
}

static void classDestructorEnd(void* ud)
{
    FRAG_UNINDENT(classDestructorEnd)
    END
}

static void classMethodBegin(void* ud, const CompilerLocation* loc, const CompilerLocation* visLoc,
    CompilerVisibility vis, const CompilerLocation* optionalStatic, const CompilerLocation* retLoc, CompilerType* ret)
{
    FRAG(classMethodBegin)
        LOC(loc)
        LOC(visLoc)
        VIS(vis)
        OPTLOC(optionalStatic)
        LOC(retLoc)
        TYPE(ret)
    END_INDENT
}

static void classMethodNameSimple(void* ud, const CompilerLocation* loc, const char* name)
{
    FRAG(classMethodNameSimple)
        LOC(loc)
        STR(name)
    END
}

static void classMethodNameArg(void* ud, const CompilerLocation* loc,
    const char* name, CompilerType* type, const CompilerLocation* argLoc, const char* arg)
{
    FRAG(classMethodNameArg)
        LOC(loc)
        STR(name)
        TYPE(type)
        LOC(argLoc)
        STR(arg)
    END
}

static void classMethodNameEnd(void* ud,
    const CompilerLocation* optionalFinal, const CompilerLocation* optionalOverride)
{
    FRAG_UNINDENT(classMethodNameEnd)
        OPTLOC(optionalFinal)
        OPTLOC(optionalOverride)
        END
}

static void classMethodEnd_Abstract(void* ud, const CompilerLocation* loc)
{
    FRAG_UNINDENT(classMethodEnd_Abstract)
        LOC(loc)
    END
}

static void classMethodBodyBegin(void* ud)
{
    FRAG(classMethodBodyBegin)
    END_INDENT
}

static void classMethodEnd(void* ud)
{
    FRAG_UNINDENT(classMethodEnd)
    END
}

static void classMembersEnd(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode != PARSE_ATTR && g_parseMode != PARSE_TYPES && g_parseMode != PARSE_EXPR) {
        FRAG_UNINDENT(classMembersEnd)
            LOC(loc)
        END
    }
}

static void classEnd(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode != PARSE_ATTR && g_parseMode != PARSE_TYPES && g_parseMode != PARSE_EXPR) {
        FRAG_UNINDENT(classEnd)
            LOC(loc)
        END
    }
}

static void varDeclBegin(void* ud, const CompilerLocation* loc, const CompilerLocation* optionalStatic, bool isConst)
{
    FRAG(varDeclBegin)
        LOC(loc)
        OPTLOC(optionalStatic)
        BOOL(isConst)
    END_INDENT
}

static void varBegin(void* ud, const CompilerLocation* loc, const char* name)
{
    if (g_parseMode != PARSE_TYPES && g_parseMode != PARSE_EXPR) {
        FRAG(varBegin)
            LOC(loc)
            STR(name)
        END_INDENT
    }
}

static void varType(void* ud, const CompilerLocation* loc, CompilerType* type)
{
    if (g_parseMode == PARSE_TYPES && g_parseMode != PARSE_EXPR) {
        FRAG(result)
            LOC(loc)
            TYPE(type)
        END
    } else {
        FRAG(varType)
            LOC(loc)
            TYPE(type)
        END
    }
}

static void varType_Array(void* ud, const CompilerLocation* loc, CompilerType* elementType, CompilerExpr* size)
{
    FRAG(varType_Array)
        LOC(loc)
        TYPE(elementType)
        EXPR(size)
    END
}

static void varInitializer(void* ud, CompilerExpr* value)
{
    if (g_parseMode == PARSE_EXPR) {
        FRAG(result)
            EXPR(value)
        END
    } else {
        FRAG(varInitializer)
            EXPR(value)
        END
    }
}

static void varEnd(void* ud)
{
    if (g_parseMode != PARSE_TYPES && g_parseMode != PARSE_EXPR) {
        FRAG_UNINDENT(varEnd)
        END
    }
}

static void varDeclEnd(void* ud, const CompilerLocation* loc)
{
    FRAG_UNINDENT(varDeclEnd)
        LOC(loc)
    END
}

static void structInitializerBegin(void* ud)
{
    FRAG(structInitializerBegin)
    END_INDENT
}

static void structInitializer(void* ud, const CompilerLocation* loc, const char* name, CompilerExpr* value)
{
    FRAG(structInitializer)
        LOC(loc)
        STR(name)
        EXPR(value)
    END
}

static void structInitializerEnd(void* ud)
{
    FRAG_UNINDENT(structInitializerEnd)
    END
}

static void arrayInitializerBegin(void* ud, const CompilerLocation* loc)
{
    FRAG(arrayInitializerBegin)
        LOC(loc)
    END_INDENT
}

static void arrayInitializer(void* ud, CompilerExpr* value)
{
    FRAG(arrayInitializer)
        EXPR(value)
    END
}

static void arrayInitializerEnd(void* ud, const CompilerLocation* loc)
{
    FRAG_UNINDENT(arrayInitializerEnd)
        LOC(loc)
    END
}

static CompilerType* typeVoid(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode == PARSE_TYPES) {
        FRAG(typeVoid)
            LOC(loc)
        END
    }

    return &g_void;
}

static CompilerType* typeBit(void* ud, const CompilerLocation* loc, CompilerExpr* optionalExpr)
{
    if (g_parseMode == PARSE_TYPES) {
        FRAG(typeBit)
            LOC(loc)
            OPTEXPR(optionalExpr)
        END
    }

    if (!optionalExpr)
        return &g_bit;
    else {
        ParserTestContext* context = (ParserTestContext*)ud;
        CompilerType* type = (CompilerType*)compilerTempAlloc(context->compiler, sizeof(CompilerType));
        memset(type, 0, sizeof(CompilerType));
        type->name = "bit";
        type->size = optionalExpr;
        return type;
    }
}

static CompilerType* typeBool(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode == PARSE_TYPES) {
        FRAG(typeBool)
            LOC(loc)
        END
    }

    return &g_bool;
}

static CompilerType* typeInt8(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode == PARSE_TYPES) {
        FRAG(typeInt8)
            LOC(loc)
        END
    }

    return &g_int8;
}

static CompilerType* typeUInt8(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode == PARSE_TYPES) {
        FRAG(typeUInt8)
            LOC(loc)
        END
    }

    return &g_uint8;
}

static CompilerType* typeInt16(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode == PARSE_TYPES) {
        FRAG(typeInt16)
            LOC(loc)
        END
    }

    return &g_int16;
}

static CompilerType* typeUInt16(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode == PARSE_TYPES) {
        FRAG(typeUInt16)
            LOC(loc)
        END
    }

    return &g_uint16;
}

static CompilerType* typeInt32(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode == PARSE_TYPES) {
        FRAG(typeInt32)
            LOC(loc)
        END
    }

    return &g_int32;
}

static CompilerType* typeUInt32(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode == PARSE_TYPES) {
        FRAG(typeUInt32)
            LOC(loc)
        END
    }

    return &g_uint32;
}

static CompilerType* typeObject(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode == PARSE_TYPES) {
        FRAG(typeObject)
            LOC(loc)
        END
    }

    return &g_object;
}

static CompilerType* typeIdentifier(void* ud, const CompilerLocation* loc, const char* text)
{
    ParserTestContext* ctx = (ParserTestContext*)ud;
    CompilerType* type;

    if (g_parseMode == PARSE_TYPES) {
        FRAG(typeIdentifier)
            LOC(loc)
            STR(text)
        END
    }

    type = (CompilerType*)compilerTempAlloc(ctx->compiler, sizeof(CompilerType));
    memset(type, 0, sizeof(CompilerType));
    type->name = compilerTempDupStr(ctx->compiler, text);

    return type;
}

static CompilerType* typePointer(void* ud, const CompilerLocation* loc, CompilerType* pointee)
{
    ParserTestContext* ctx = (ParserTestContext*)ud;
    CompilerType* type;

    if (g_parseMode == PARSE_TYPES) {
        FRAG(typePointer)
            LOC(loc)
            TYPE(pointee)
        END
    }

    type = (CompilerType*)compilerTempAlloc(ctx->compiler, sizeof(CompilerType));
    memset(type, 0, sizeof(CompilerType));
    type->pointee = pointee;

    return type;
}

static CompilerExpr* expr(void* ud, const CompilerLocation* loc, EnumID what)
{
    ParserTestContext* ctx = (ParserTestContext*)ud;
    CompilerExpr* expr = (CompilerExpr*)compilerTempAlloc(ctx->compiler, sizeof(CompilerExpr));
    memset(expr, 0, sizeof(CompilerExpr));
    expr->what = what;
    memcpy(&expr->loc, loc, sizeof(CompilerLocation));
    return expr;
}

static CompilerExpr* exprNull(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprNull)
            LOC(loc)
        END
    }

    return expr(ud, loc, EXPR_NULL);
}

static CompilerExpr* exprFalse(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprFalse)
            LOC(loc)
        END
    }

    return expr(ud, loc, EXPR_FALSE);
}

static CompilerExpr* exprTrue(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprTrue)
            LOC(loc)
        END
    }

    return expr(ud, loc, EXPR_TRUE);
}

static CompilerExpr* exprIdentifier(void* ud, const CompilerLocation* loc, const char* name)
{
    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprIdentifier)
            LOC(loc)
            STR(name)
        END
    }

    CompilerExpr* e = expr(ud, loc, EXPR_IDENTIFIER);
    e->identifier = name;
    return e;
}

static CompilerExpr* exprInteger(void* ud, const CompilerLocation* loc, uint_value_t value)
{
    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprInteger)
            LOC(loc)
            INT(value)
        END
    }

    CompilerExpr* e = expr(ud, loc, EXPR_INTEGER);
    e->integer = value;
    return e;
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

static void error(void* ud, const CompilerLocation* loc, const CompilerToken* token)
{
    static const char frag_name_[] = "yyerror";

    NOT_NULL(loc)
    if (!loc)
        return;

    NOT_NULL(token)
    if (!token)
        return;

    if (!loc->startLine) {
        logPrintf("%s: TEST FAILURE: %s is NULL in token location.\n", g_testName, "startLine");
        ++g_testFailCount;
    }
    if (!loc->endLine) {
        logPrintf("%s: TEST FAILURE: %s is NULL in token location.\n", g_testName, "endLine");
        ++g_testFailCount;
    }

    printF("(%d,%d-%d,%d): unexpected token: %s\n",
        (loc->startLine ? loc->startLine->number : 0),
        loc->startColumn,
        (loc->endLine ? loc->endLine->number : 0),
        loc->endColumn,
        token->name);
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
    parser->cb.enumBegin = enumBegin;
    parser->cb.enumType = enumType;
    parser->cb.enumMembersBegin = enumMembersBegin;
    parser->cb.enumMember = enumMember;
    parser->cb.enumMembersEnd = enumMembersEnd;
    parser->cb.enumEnd = enumEnd;
    parser->cb.structBegin = structBegin;
    parser->cb.structParent = structParent;
    parser->cb.structMembersBegin = structMembersBegin;
    parser->cb.structMember = structMember;
    parser->cb.structMembersEnd = structMembersEnd;
    parser->cb.structEnd = structEnd;
    parser->cb.structBegin = structBegin;
    parser->cb.structParent = structParent;
    parser->cb.structMembersBegin = structMembersBegin;
    parser->cb.structMember = structMember;
    parser->cb.structMembersEnd = structMembersEnd;
    parser->cb.structEnd = structEnd;
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
    parser->cb.classEnd = classEnd;
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
    parser->cb.error = error;
}
