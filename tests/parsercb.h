
struct CompilerType
{
    const char* name;
    CompilerType* pointee;
    CompilerExpr* size;
};

static CompilerType g_void = { "void", NULL, NULL };
static CompilerType g_bit = { "bit", NULL, NULL };
static CompilerType g_bool = { "bool", NULL, NULL };
static CompilerType g_int8 = { "i8", NULL, NULL };
static CompilerType g_uint8 = { "u8", NULL, NULL };
static CompilerType g_int16 = { "i16", NULL, NULL };
static CompilerType g_uint16 = { "u16", NULL, NULL };
static CompilerType g_int32 = { "i32", NULL, NULL };
static CompilerType g_uint32 = { "u32", NULL, NULL };
static CompilerType g_object = { "object", NULL, NULL };

/*==================================================================================================================*/

typedef enum EnumID {
    EXPR_NULL,
    EXPR_FALSE,
    EXPR_TRUE,
    EXPR_IDENTIFIER,
    EXPR_INTEGER,
    EXPR_PARENTHESES,
    EXPR_SUBSCRIPT,
    EXPR_MEMBER,
    EXPR_POSTFIX_INCR,
    EXPR_POSTFIX_DECR,
    EXPR_PREFIX_INCR,
    EXPR_PREFIX_DECR,
    EXPR_TAKE_ADDRESS,
    EXPR_DEREF,
    EXPR_UNARY_PLUS,
    EXPR_UNARY_MINUS,
    EXPR_BITWISE_NOT,
    EXPR_LOGIC_NOT,
    EXPR_SIZEOF,
    EXPR_MULTIPLY,
    EXPR_DIVIDE,
    EXPR_MODULO,
    EXPR_ADD,
    EXPR_SUBTRACT,
    EXPR_SHIFT_LEFT,
    EXPR_SHIFT_RIGHT,
    EXPR_GREATER,
    EXPR_GREATER_EQ,
    EXPR_LESS,
    EXPR_LESS_EQ,
    EXPR_EQUAL,
    EXPR_NOT_EQUAL,
    EXPR_BITWISE_AND,
    EXPR_BITWISE_XOR,
    EXPR_BITWISE_OR,
    EXPR_LOGIC_AND,
    EXPR_LOGIC_OR,
    EXPR_ASSIGN,
    EXPR_ASSIGN_ADD,
    EXPR_ASSIGN_SUB,
    EXPR_ASSIGN_MUL,
    EXPR_ASSIGN_DIV,
    EXPR_ASSIGN_MOD,
    EXPR_ASSIGN_AND,
    EXPR_ASSIGN_OR,
    EXPR_ASSIGN_XOR,
    EXPR_ASSIGN_SHL,
    EXPR_ASSIGN_SHR
} EnumID;

struct CompilerExpr
{
    EnumID what;
    CompilerLocation loc;
    const char* identifier;
    uint_value_t integer;
    CompilerExpr* operand;
    CompilerExpr* array;
    CompilerExpr* index;
    CompilerExpr* op1;
    CompilerExpr* op2;
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

#endif /* NDEBUG */

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define ASSERT_NAME_IS(NAME) assert(!strcmp(__func__, NAME))
#elif defined(_MSC_VER) /*|| (defined(__GNUC__) && __GNUC__ >= 2)*/
#define ASSERT_NAME_IS(NAME) assert(!strcmp(__FUNCTION__, NAME))
#else
#define ASSERT_NAME_IS(NAME) ((void)0)
#endif

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
    #define _(STRING) lua_pushliteral(L, STRING)
    #define E(MEMBER) printExpr(L, expr->MEMBER)

    switch (expr->what) {
        case EXPR_NULL: _("null"); return;
        case EXPR_FALSE: _("false"); return;
        case EXPR_TRUE: _("true"); return;
        case EXPR_IDENTIFIER: lua_pushstring(L, expr->identifier); return;
        case EXPR_INTEGER: pushHex(L, expr->integer); return;
        case EXPR_PARENTHESES: _("("); E(operand); _(")"); return;
        case EXPR_SUBSCRIPT: E(array); _("["); E(index); _("]"); return;
        case EXPR_MEMBER: E(operand); _("."); lua_pushstring(L, expr->identifier); return;
        case EXPR_POSTFIX_INCR: E(operand); _("++"); return;
        case EXPR_POSTFIX_DECR: E(operand); _("--"); return;
        case EXPR_PREFIX_INCR: _("++"); E(operand); return;
        case EXPR_PREFIX_DECR: _("--"); E(operand); return;
        case EXPR_TAKE_ADDRESS: _("&"); E(operand); return;
        case EXPR_DEREF: _("*"); E(operand); return;
        case EXPR_UNARY_PLUS: _("+"); E(operand); return;
        case EXPR_UNARY_MINUS: _("-"); E(operand); return;
        case EXPR_BITWISE_NOT: _("~"); E(operand); return;
        case EXPR_LOGIC_NOT: _("!"); E(operand); return;
        case EXPR_SIZEOF: _("sizeof("); E(operand); _(")"); return;
        case EXPR_MULTIPLY: E(op1); _("*"); E(op2); return;
        case EXPR_DIVIDE: E(op1); _("/"); E(op2); return;
        case EXPR_MODULO: E(op1); _("%"); E(op2); return;
        case EXPR_ADD: E(op1); _("+"); E(op2); return;
        case EXPR_SUBTRACT: E(op1); _("-"); E(op2); return;
        case EXPR_SHIFT_LEFT: E(op1); _("<<"); E(op2); return;
        case EXPR_SHIFT_RIGHT: E(op1); _(">>"); E(op2); return;
        case EXPR_GREATER: E(op1); _(">"); E(op2); return;
        case EXPR_GREATER_EQ: E(op1); _(">="); E(op2); return;
        case EXPR_LESS: E(op1); _("<"); E(op2); return;
        case EXPR_LESS_EQ: E(op1); _("<="); E(op2); return;
        case EXPR_EQUAL: E(op1); _("=="); E(op2); return;
        case EXPR_NOT_EQUAL: E(op1); _("!="); E(op2); return;
        case EXPR_BITWISE_AND: E(op1); _("&"); E(op2); return;
        case EXPR_BITWISE_XOR: E(op1); _("^"); E(op2); return;
        case EXPR_BITWISE_OR: E(op1); _("|"); E(op2); return;
        case EXPR_LOGIC_AND: E(op1); _("&&"); E(op2); return;
        case EXPR_LOGIC_OR: E(op1); _("||"); E(op2); return;
        case EXPR_ASSIGN: E(op1); _("="); E(op2); return;
        case EXPR_ASSIGN_ADD: E(op1); _("+="); E(op2); return;
        case EXPR_ASSIGN_SUB: E(op1); _("-="); E(op2); return;
        case EXPR_ASSIGN_MUL: E(op1); _("*="); E(op2); return;
        case EXPR_ASSIGN_DIV: E(op1); _("/="); E(op2); return;
        case EXPR_ASSIGN_MOD: E(op1); _("%="); E(op2); return;
        case EXPR_ASSIGN_AND: E(op1); _("&="); E(op2); return;
        case EXPR_ASSIGN_OR: E(op1); _("|="); E(op2); return;
        case EXPR_ASSIGN_XOR: E(op1); _("^="); E(op2); return;
        case EXPR_ASSIGN_SHL: E(op1); _("<<="); E(op2); return;
        case EXPR_ASSIGN_SHR: E(op1); _(">>="); E(op2); return;
    }

    #undef E
    #undef _

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

#define FRAG_CUSTOM_NAME(NAME) \
    { \
        ParserTestContext* context = (ParserTestContext*)ud; \
        lua_State* L = context->L; \
        const char* frag_name_ = #NAME; \
        int arg_n_ = 1; \
        lua_pushstring(L, frag_name_); \

#define FRAG(NAME) \
    FRAG_CUSTOM_NAME(NAME) \
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
            lua_pushfstring(L, " %s:%s", #NAME, "<none>"); \
        else \
            lua_pushfstring(L, " %s:\"%s\"", #NAME, NAME);

#define STR(NAME) \
        NOT_NULL(NAME) \
        OPTSTR(NAME)

#define OPTLOC(NAME) \
        ++arg_n_; \
        if (!NAME) \
            lua_pushfstring(L, " %s:%s", #NAME, "<none>"); \
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
            lua_pushfstring(L, " %s:%s", #NAME, "<none>"); \
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
            lua_pushfstring(L, " %s:%s", #NAME, "<none>"); \
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
        FRAG_CUSTOM_NAME(enum)
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
        FRAG_CUSTOM_NAME(struct)
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
    if (g_parseMode != PARSE_ATTR && g_parseMode != PARSE_TYPES
            && g_parseMode != PARSE_EXPR && g_parseMode != PARSE_STMT) {
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
    if (g_parseMode != PARSE_ATTR && g_parseMode != PARSE_TYPES
            && g_parseMode != PARSE_EXPR && g_parseMode != PARSE_STMT) {
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
    if (g_parseMode != PARSE_STMT) {
        FRAG(classMethodBegin)
            LOC(loc)
            LOC(visLoc)
            VIS(vis)
            OPTLOC(optionalStatic)
            LOC(retLoc)
            TYPE(ret)
        END_INDENT
    }
}

static void classMethodNameSimple(void* ud, const CompilerLocation* loc, const char* name)
{
    if (g_parseMode != PARSE_STMT) {
        FRAG(classMethodNameSimple)
            LOC(loc)
            STR(name)
        END
    }
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
    if (g_parseMode != PARSE_STMT) {
        FRAG_UNINDENT(classMethodNameEnd)
            OPTLOC(optionalFinal)
            OPTLOC(optionalOverride)
        END
    }
}

static void classMethodEnd_Abstract(void* ud, const CompilerLocation* loc)
{
    FRAG_UNINDENT(classMethodEnd_Abstract)
        LOC(loc)
    END
}

static void classMethodBodyBegin(void* ud)
{
    if (g_parseMode != PARSE_STMT) {
        FRAG(classMethodBodyBegin)
        END_INDENT
    }
}

static void classMethodEnd(void* ud)
{
    if (g_parseMode != PARSE_STMT) {
        FRAG_UNINDENT(classMethodEnd)
        END
    }
}

static void classMembersEnd(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode != PARSE_ATTR && g_parseMode != PARSE_TYPES
            && g_parseMode != PARSE_EXPR && g_parseMode != PARSE_STMT) {
        FRAG_UNINDENT(classMembersEnd)
            LOC(loc)
        END
    }
}

static void classEnd(void* ud, const CompilerLocation* loc)
{
    if (g_parseMode != PARSE_ATTR && g_parseMode != PARSE_TYPES
            && g_parseMode != PARSE_EXPR && g_parseMode != PARSE_STMT) {
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
        FRAG_CUSTOM_NAME(result)
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
        FRAG_CUSTOM_NAME(result)
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
    CompilerExpr* e = (CompilerExpr*)compilerTempAlloc(ctx->compiler, sizeof(CompilerExpr));
    memset(e, 0, sizeof(CompilerExpr));
    e->what = what;
    memcpy(&e->loc, loc, sizeof(CompilerLocation));
    return e;
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
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprIdentifier)
            LOC(loc)
            STR(name)
        END
    }

    e = expr(ud, loc, EXPR_IDENTIFIER);
    e->identifier = name;
    return e;
}

static CompilerExpr* exprInteger(void* ud, const CompilerLocation* loc, uint_value_t value)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprInteger)
            LOC(loc)
            INT(value)
        END
    }

    e = expr(ud, loc, EXPR_INTEGER);
    e->integer = value;
    return e;
}

static CompilerExpr* exprParentheses(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprParentheses)
            LOC(loc)
            EXPR(operand)
        END
    }

    e = expr(ud, loc, EXPR_PARENTHESES);
    e->operand = operand;
    return e;
}

static void exprNewBegin(void* ud, const CompilerLocation* loc, CompilerType* type)
{
    /* FIXME */
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(type);
}

static CompilerExpr* exprNewEnd_Struct(void* ud, const CompilerLocation* loc)
{
    /* FIXME */
    UNUSED(ud);
    UNUSED(loc);
    return NULL;
}

static CompilerExpr* exprNewEnd_Array(void* ud, const CompilerLocation* loc, CompilerExpr* size)
{
    /* FIXME */
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(size);
    return NULL;
}

static void exprMethodCallBegin(void* ud, CompilerExpr* callee)
{
    /* FIXME */
    UNUSED(ud);
    UNUSED(callee);
}

static void exprMethodSimple(void* ud, const CompilerLocation* loc, const char* name)
{
    /* FIXME */
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
}

static void exprMethodArg(void* ud, const CompilerLocation* loc, const char* name, CompilerExpr* value)
{
    /* FIXME */
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(name);
    UNUSED(value);
}

static CompilerExpr* exprMethodCallEnd(void* ud, const CompilerLocation* loc)
{
    /* FIXME */
    UNUSED(ud);
    UNUSED(loc);
    return NULL;
}

static CompilerExpr* exprSubscript(void* ud, const CompilerLocation* loc, CompilerExpr* arr, CompilerExpr* idx)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprSubscript)
            LOC(loc)
            EXPR(arr)
            EXPR(idx)
        END
    }

    e = expr(ud, loc, EXPR_SUBSCRIPT);
    e->array = arr;
    e->index = idx;
    return e;
}

static CompilerExpr* exprMember(void* ud,
    const CompilerLocation* loc, CompilerExpr* target, const CompilerLocation* nameLoc, const char* name)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprMember)
            LOC(loc)
            EXPR(target)
            LOC(nameLoc)
            STR(name)
        END
    }

    e = expr(ud, loc, EXPR_MEMBER);
    e->operand = target;
    e->identifier = name;
    return e;
}

static CompilerExpr* exprPostfixIncr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprPostfixIncr)
            LOC(loc)
            EXPR(operand)
        END
    }

    e = expr(ud, loc, EXPR_POSTFIX_INCR);
    e->operand = operand;
    return e;
}

static CompilerExpr* exprPostfixDecr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprPostfixDecr)
            LOC(loc)
            EXPR(operand)
        END
    }

    e = expr(ud, loc, EXPR_POSTFIX_DECR);
    e->operand = operand;
    return e;
}

static CompilerExpr* exprPrefixIncr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprPrefixIncr)
            LOC(loc)
            EXPR(operand)
        END
    }

    e = expr(ud, loc, EXPR_PREFIX_INCR);
    e->operand = operand;
    return e;
}

static CompilerExpr* exprPrefixDecr(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprPrefixDecr)
            LOC(loc)
            EXPR(operand)
        END
    }

    e = expr(ud, loc, EXPR_PREFIX_DECR);
    e->operand = operand;
    return e;
}

static CompilerExpr* exprTakeAddress(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprTakeAddress)
            LOC(loc)
            EXPR(operand)
        END
    }

    e = expr(ud, loc, EXPR_TAKE_ADDRESS);
    e->operand = operand;
    return e;
}

static CompilerExpr* exprDeref(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprDeref)
            LOC(loc)
            EXPR(operand)
        END
    }

    e = expr(ud, loc, EXPR_DEREF);
    e->operand = operand;
    return e;
}

static CompilerExpr* exprUnaryPlus(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprUnaryPlus)
            LOC(loc)
            EXPR(operand)
        END
    }

    e = expr(ud, loc, EXPR_UNARY_PLUS);
    e->operand = operand;
    return e;
}

static CompilerExpr* exprUnaryMinus(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprUnaryMinus)
            LOC(loc)
            EXPR(operand)
        END
    }

    e = expr(ud, loc, EXPR_UNARY_MINUS);
    e->operand = operand;
    return e;
}

static CompilerExpr* exprBitwiseNot(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprBitwiseNot)
            LOC(loc)
            EXPR(operand)
        END
    }

    e = expr(ud, loc, EXPR_BITWISE_NOT);
    e->operand = operand;
    return e;
}

static CompilerExpr* exprLogicNot(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprLogicNot)
            LOC(loc)
            EXPR(operand)
        END
    }

    e = expr(ud, loc, EXPR_LOGIC_NOT);
    e->operand = operand;
    return e;
}

static CompilerExpr* exprSizeOf(void* ud, const CompilerLocation* loc, CompilerExpr* operand)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprSizeOf)
            LOC(loc)
            EXPR(operand)
        END
    }

    e = expr(ud, loc, EXPR_SIZEOF);
    e->operand = operand;
    return e;
}

static CompilerExpr* exprMultiply(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprMultiply)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_MULTIPLY);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprDivide(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprDivide)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_DIVIDE);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprModulo(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprModulo)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_MODULO);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprAdd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprAdd)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_ADD);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprSubtract(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprSubtract)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_SUBTRACT);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprShiftLeft(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprShiftLeft)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_SHIFT_LEFT);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprShiftRight(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprShiftRight)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_SHIFT_RIGHT);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprGreater(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprGreater)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_GREATER);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprGreaterEq(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprGreaterEq)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_GREATER_EQ);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprLess(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprLess)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_LESS);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprLessEq(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprLessEq)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_LESS_EQ);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprEqual(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprEqual)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_EQUAL);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprNotEqual(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprNotEqual)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_NOT_EQUAL);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprBitwiseAnd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprBitwiseAnd)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_BITWISE_AND);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprBitwiseXor(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprBitwiseXor)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_BITWISE_XOR);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprBitwiseOr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprBitwiseOr)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_BITWISE_OR);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprLogicAnd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprLogicAnd)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_LOGIC_AND);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprLogicOr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprLogicOr)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_LOGIC_OR);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprTernary(void* ud, const CompilerLocation* loc,
    CompilerExpr* cond, CompilerExpr* trueValue, CompilerExpr* falseValue)
{
    /* FIXME */
    UNUSED(ud);
    UNUSED(loc);
    UNUSED(cond);
    UNUSED(trueValue);
    UNUSED(falseValue);
    return NULL;
}

static CompilerExpr* exprAssign(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprAssign)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_ASSIGN);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprAssignAdd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprAssignAdd)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_ASSIGN_ADD);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprAssignSub(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprAssignSub)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_ASSIGN_SUB);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprAssignMul(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprAssignMul)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_ASSIGN_MUL);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprAssignDiv(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprAssignDiv)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_ASSIGN_DIV);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprAssignMod(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprAssignMod)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_ASSIGN_MOD);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprAssignAnd(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprAssignAnd)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_ASSIGN_AND);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprAssignOr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprAssignOr)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_ASSIGN_OR);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprAssignXor(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprAssignXor)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_ASSIGN_XOR);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprAssignShl(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprAssignShl)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_ASSIGN_SHL);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}

static CompilerExpr* exprAssignShr(void* ud, const CompilerLocation* loc, CompilerExpr* op1, CompilerExpr* op2)
{
    CompilerExpr* e;

    if (g_parseMode == PARSE_EXPR) {
        FRAG(exprAssignShr)
            LOC(loc)
            EXPR(op1)
            EXPR(op2)
        END
    }

    e = expr(ud, loc, EXPR_ASSIGN_SHR);
    e->op1 = op1;
    e->op2 = op2;
    return e;
}


static void stmtEmpty(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtEmpty)
        LOC(loc)
    END
}

static void stmtExpr(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    FRAG(stmtExpr)
        LOC(loc)
        EXPR(e)
    END
}

static void stmtLabel(void* ud, const CompilerLocation* loc, const char* name)
{
    FRAG(stmtLabel)
        LOC(loc)
        STR(name)
    END
}

static void stmtGoto(void* ud, const CompilerLocation* loc, const CompilerLocation* nameLoc, const char* name)
{
    FRAG(stmtGoto)
        LOC(loc)
        LOC(nameLoc)
        STR(name)
    END
}

static void stmtBreak(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtBreak)
        LOC(loc)
    END
}

static void stmtContinue(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtContinue)
        LOC(loc)
    END
}

static void stmtDelete(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    FRAG(stmtDelete)
        LOC(loc)
        EXPR(e)
    END
}

static void stmtThrow(void* ud, const CompilerLocation* loc, CompilerExpr* optionalExpr)
{
    FRAG(stmtThrow)
        LOC(loc)
        OPTEXPR(optionalExpr)
    END
}

static void stmtCompoundBegin(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtCompoundBegin)
        LOC(loc)
    END
}

static void stmtCompoundEnd(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtCompoundEnd)
        LOC(loc)
    END
}

static void stmtIfBegin(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtIfBegin)
        LOC(loc)
    END
}

static void stmtIfThen(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    FRAG(stmtIfThen)
        LOC(loc)
        EXPR(e)
    END
}

static void stmtIfElse(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtIfElse)
        LOC(loc)
    END
}

static void stmtIfEnd(void* ud)
{
    FRAG(stmtIfEnd)
    END
}

static void stmtWhileBegin(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtWhileBegin)
        LOC(loc)
    END
}

static void stmtWhileDo(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    FRAG(stmtWhileDo)
        LOC(loc)
        EXPR(e)
    END
}

static void stmtWhileEnd(void* ud)
{
    FRAG(stmtWhileEnd)
    END
}

static void stmtDoBegin(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtDoBegin)
        LOC(loc)
    END
}

static void stmtDoEnd(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    FRAG(stmtDoEnd)
        LOC(loc)
        EXPR(e)
    END
}

static void stmtForBegin(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtForBegin)
        LOC(loc)
    END
}

static void stmtForInit(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    FRAG(stmtForInit)
        LOC(loc)
        EXPR(e)
    END
}

static void stmtForCondition(void* ud, CompilerExpr* e)
{
    FRAG(stmtForCondition)
        EXPR(e)
    END
}

static void stmtForUpdate(void* ud, CompilerExpr* e)
{
    FRAG(stmtForUpdate)
        EXPR(e)
    END
}

static void stmtForDo(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtForDo)
        LOC(loc)
    END
}

static void stmtForEnd(void* ud)
{
    FRAG(stmtForEnd)
    END
}

static void stmtSwitchBegin(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtSwitchBegin)
        LOC(loc)
    END
}

static void stmtSwitchOperand(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    FRAG(stmtSwitchOperand)
        LOC(loc)
        EXPR(e)
    END
}

static void stmtSwitchCase(void* ud, const CompilerLocation* loc, CompilerExpr* e)
{
    FRAG(stmtSwitchCase)
        LOC(loc)
        EXPR(e)
    END
}

static void stmtSwitchCaseRange(void* ud, const CompilerLocation* loc, CompilerExpr* start, CompilerExpr* end)
{
    FRAG(stmtSwitchCaseRange)
        LOC(loc)
        EXPR(start)
        EXPR(end)
    END
}

static void stmtSwitchDefault(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtSwitchDefault)
        LOC(loc)
    END
}

static void stmtSwitchEnd(void* ud)
{
    FRAG(stmtSwitchEnd)
    END
}

static void stmtTryBegin(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtTryBegin)
        LOC(loc)
    END
}

static void stmtTry_CatchBegin(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtTry_CatchBegin)
        LOC(loc)
    END
}

static void stmtTry_CatchDo(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtTry_CatchDo)
        LOC(loc)
    END
}

static void stmtTry_CatchEnd(void* ud)
{
    FRAG(stmtTry_CatchEnd)
    END
}

static void stmtTry_FinallyBegin(void* ud, const CompilerLocation* loc)
{
    FRAG(stmtTry_FinallyBegin)
        LOC(loc)
    END
}

static void stmtTry_FinallyEnd(void* ud)
{
    FRAG(stmtTry_FinallyEnd)
    END
}

static void stmtTryEnd(void* ud)
{
    FRAG(stmtTryEnd)
    END
}

static void error(void* ud, const CompilerLocation* loc, const CompilerToken* token)
{
    static const char frag_name_[] = "yyerror";

    UNUSED(ud);

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
