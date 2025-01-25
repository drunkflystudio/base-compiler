%{
#include <compiler/private.h>

#ifdef BORLAND
#pragma option -w-pia
#pragma option -w-aus
#pragma option -w-rch
#endif

#define YYREDUCEPOSNFUNC(ret, terms, term_vals, term_no, stk_pos, yychar, yyposn, extra) \
    yyreduceposn(&(ret), (terms), (term_no))

void yyreduceposn(Location* ret, const Location* inputs, int count);

%}

%token <token> KW_abstract
%token <token> KW_bool
%token <token> KW_bit
%token <token> KW_break
%token <token> KW_byte
%token <token> KW_case
%token <token> KW_catch
%token <token> KW_class
%token <token> KW_const
%token <token> KW_continue
%token <token> KW_default
%token <token> KW_delete
%token <token> KW_do
%token <token> KW_else
%token <token> KW_enum
%token <token> KW_false
%token <token> KW_final
%token <token> KW_finally
%token <token> KW_flags
%token <token> KW_for
%token <token> KW_friend
%token <token> KW_goto
%token <token> KW_if
%token <token> KW_int
%token <token> KW_int8
%token <token> KW_int16
%token <token> KW_int32
%token <token> KW_interface
%token <token> KW_new
%token <token> KW_null
%token <token> KW_object
%token <token> KW_override
%token <token> KW_private
%token <token> KW_protected
%token <token> KW_public
%token <token> KW_sbyte
%token <token> KW_sizeof
%token <token> KW_static
%token <token> KW_struct
%token <token> KW_switch
%token <token> KW_throw
%token <token> KW_true
%token <token> KW_try
%token <token> KW_uint8
%token <token> KW_uint16
%token <token> KW_uint32
%token <token> KW_union
%token <token> KW_var
%token <token> KW_void
%token <token> KW_while
%token <token> KW_word
%token <token> T_FILE_START
%token <token> T_MULTI_LINE_COMMENT
%token <token> T_SINGLE_LINE_COMMENT
%token <token> T_IDENTIFIER
%token <token> T_INTEGER_LITERAL
%token <token> T_DOT
%token <token> T_DOUBLE_DOT
%token <token> T_LBRACKET
%token <token> T_RBRACKET
%token <token> T_INCR
%token <token> T_DECR
%token <token> T_SEMICOLON
%token <token> T_AMPERSAND
%token <token> T_ASTERISK
%token <token> T_PLUS
%token <token> T_MINUS
%token <token> T_TILDE
%token <token> T_EXCLAMATION
%token <token> T_LPAREN
%token <token> T_RPAREN
%token <token> T_SLASH
%token <token> T_PERCENT
%token <token> T_LSHIFT
%token <token> T_RSHIFT
%token <token> T_LESS
%token <token> T_GREATER
%token <token> T_LESS_EQ
%token <token> T_GREATER_EQ
%token <token> T_EQUAL
%token <token> T_NOT_EQUAL
%token <token> T_CARET
%token <token> T_VBAR
%token <token> T_DOUBLE_AMPERSAND
%token <token> T_DOUBLE_VBAR
%token <token> T_QUESTION
%token <token> T_COLON
%token <token> T_ASSIGN
%token <token> T_ADD_ASSIGN
%token <token> T_SUB_ASSIGN
%token <token> T_MUL_ASSIGN
%token <token> T_DIV_ASSIGN
%token <token> T_MOD_ASSIGN
%token <token> T_OR_ASSIGN
%token <token> T_AND_ASSIGN
%token <token> T_XOR_ASSIGN
%token <token> T_SHL_ASSIGN
%token <token> T_SHR_ASSIGN
%token <token> T_LCURLY
%token <token> T_RCURLY
%token <token> T_COMMA
%token <token> T_ATSIGN
%token <token> T_KWdo_WITH_LCURLY

%location Location {
    struct Module* module;
    int startLine;
    int startColumn;
    int endLine;
    int endColumn;
}

%start translation_unit
%%

translation_unit
    : /* empty */
    ;

/*********************************************************************************************************************/

%%

void yyreduceposn(Location* ret, const Location* inputs, int count)
{
    UNUSED(ret);
    UNUSED(inputs);
    UNUSED(count);
}

void yyerror(yycontext* yyctx, int yychar, const YYSTYPE* yylval, const YYPOSN* yyposn)
{
    UNUSED(yyctx);
    UNUSED(yychar);
    UNUSED(yylval);
    UNUSED(yyposn);
}

int yylex(yycontext* yyctx)
{
    UNUSED(yyctx);
    return T_EOF;
}
