%{
#include <drunkfly/compiler/parser.h>
#include <drunkfly/compiler/private.h>
#include <drunkfly/compiler/token.h>
#include <drunkfly/compiler/bundle.h>
#include <lauxlib.h>

#ifdef BORLAND
#pragma option -w-pia
#pragma option -w-aus
#pragma option -w-rch
#endif

#define merge(LOC1, LOC2) (compilerMergeLocations(COMPILER, &(LOC1), &(LOC2)))
#define YYREDUCEPOSNFUNC(ret, terms, term_vals, term_no, stk_pos, yychar, yyposn, extra) \
    yyreduceposn(COMPILER, &(ret), (terms), (term_no))

#define CTX ((CompilerParser*)(ctx->userdata))
#define COMPILER (CTX->compiler)
#define BUNDLE (COMPILER->bundle)
#define CB (CTX->cb)
#define UD (CTX->cb.ud)

void yyreduceposn(Compiler* compiler, CompilerLocation* ret, const CompilerLocation* inputs, int count);

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
%token <token> KW_extern
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
%token <token> KW_return
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
%token <token> T_INVALID_SYMBOL
%token <token> T_INVALID_BINARY_LITERAL
%token <token> T_INVALID_OCTAL_LITERAL
%token <token> T_INVALID_HEXADECIMAL_LITERAL
%token <token> T_INVALID_DECIMAL_LITERAL
%token <token> T_MULTI_LINE_COMMENT
%token <token> T_SINGLE_LINE_COMMENT
%token <token> T_IDENTIFIER
%token <token> T_INTEGER_LITERAL
%token <token> T_DOT
%token <token> T_DBLDOT
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
%token <token> T_DBLAMPERSAND
%token <token> T_DBLVBAR
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
%token <token> T_KWbit_WITH_LBRACKET

%type <expr> primary_expression
%type <expr> postfix_expression
%type <expr> unary_expression
%type <expr> multiplicative_expression
%type <expr> additive_expression
%type <expr> shift_expression
%type <expr> relational_expression
%type <expr> equality_expression
%type <expr> and_expression
%type <expr> exclusive_or_expression
%type <expr> inclusive_or_expression
%type <expr> logical_and_expression
%type <expr> logical_or_expression
%type <expr> conditional_expression
%type <expr> assignment_expression
%type <expr> expression
%type <expr> optional_expression
%type <expr> statement_switch_expr
%type <type> type_name
%type <flag> var_or_const
%type <expr> variable_declaration_or_expression
%type <flag> optional_final
%type <visi> class_member_visibility
%type <flag> optional_static
%type <flag> optional_override
%type <flag> enum_or_flags
%type <flag> struct_or_union

%union {
    CompilerType* type;
    CompilerToken* token;
    CompilerExpr* expr;
    CompilerVisibility visi;
    bool flag;
}

%location CompilerLocation {
    SourceFile* file;
    SourceLine* startLine;
    SourceLine* endLine;
    int startColumn;
    int endColumn;
}

%start translation_unit
%%

/*********************************************************************************************************************/
/* Expressions */

primary_expression
    : KW_null { $$ = CB.exprNull(UD, &@1); }
    | KW_false { $$ = CB.exprFalse(UD, &@1); }
    | KW_true { $$ = CB.exprTrue(UD, &@1); }
    | T_IDENTIFIER { $$ = CB.exprIdentifier(UD, &@1, $1->text); }
    | T_INTEGER_LITERAL { $$ = CB.exprInteger(UD, &@1, $1->integer); }
    | operator_new struct_initializer { $$ = CB.exprNewEnd_Struct(UD, merge(@1, @2)); }
    | operator_new T_LBRACKET expression T_RBRACKET { $$ = CB.exprNewEnd_Array(UD, merge(@1, @4), $3); }
    | method_call_begin method_call_args T_RBRACKET { $$ = CB.exprMethodCallEnd(UD, merge(@1, @3)); }
    | T_LPAREN expression T_RPAREN { $$ = CB.exprParentheses(UD, merge(@1, @3), $2); }
    ;

operator_new : KW_new type_name { CB.exprNewBegin(UD, &@1, $2); };

method_call_begin : T_LBRACKET expression { CB.exprMethodCallBegin(UD, $2); };
method_call_args : method_call_single_arg | method_call_arg_list;
method_call_arg_list : method_call_arg | method_call_arg_list method_call_arg;
method_call_single_arg : T_IDENTIFIER { CB.exprMethodSimple(UD, &@1, $1->text); };
method_call_arg : T_IDENTIFIER T_COLON expression { CB.exprMethodArg(UD, merge(@1, @2), $1->text, $3); };

postfix_expression
    : primary_expression { $$ = $1; }
    | postfix_expression T_LBRACKET expression T_RBRACKET { $$ = CB.exprSubscript(UD, merge(@1, @4), $1, $3); }
    | postfix_expression T_DOT T_IDENTIFIER { $$ = CB.exprMember(UD, merge(@1, @3), $1, &@3, $3->text); }
    | postfix_expression T_INCR { $$ = CB.exprPostfixIncr(UD, merge(@1, @2), $1); }
    | postfix_expression T_DECR { $$ = CB.exprPostfixDecr(UD, merge(@1, @2), $1); }
    ;

unary_expression
    : postfix_expression { $$ = $1; }
    | T_INCR unary_expression { $$ = CB.exprPrefixIncr(UD, merge(@1, @2), $2); }
    | T_DECR unary_expression { $$ = CB.exprPrefixDecr(UD, merge(@1, @2), $2); }
    | T_AMPERSAND unary_expression { $$ = CB.exprTakeAddress(UD, merge(@1, @2), $2); }
    | T_ASTERISK unary_expression { $$ = CB.exprDeref(UD, merge(@1, @2), $2); }
    | T_PLUS unary_expression { $$ = CB.exprUnaryPlus(UD, merge(@1, @2), $2); }
    | T_MINUS unary_expression { $$ = CB.exprUnaryMinus(UD, merge(@1, @2), $2); }
    | T_TILDE unary_expression { $$ = CB.exprBitwiseNot(UD, merge(@1, @2), $2); }
    | T_EXCLAMATION unary_expression { $$ = CB.exprLogicNot(UD, merge(@1, @2), $2); }
    | KW_sizeof T_LPAREN expression T_RPAREN { $$ = CB.exprSizeOf(UD, merge(@1, @4), $3); }
    ;

multiplicative_expression
    : unary_expression { $$ = $1; }
    | multiplicative_expression T_ASTERISK unary_expression { $$ = CB.exprMultiply(UD, merge(@1, @3), $1, $3); }
    | multiplicative_expression T_SLASH unary_expression { $$ = CB.exprDivide(UD, merge(@1, @3), $1, $3); }
    | multiplicative_expression T_PERCENT unary_expression { $$ = CB.exprModulo(UD, merge(@1, @3), $1, $3); }
    ;

additive_expression
    : multiplicative_expression { $$ = $1; }
    | additive_expression T_PLUS multiplicative_expression { $$ = CB.exprAdd(UD, merge(@1, @3), $1, $3); }
    | additive_expression T_MINUS multiplicative_expression { $$ = CB.exprSubtract(UD, merge(@1, @3), $1, $3); }
    ;

shift_expression
    : additive_expression { $$ = $1; }
    | shift_expression T_LSHIFT additive_expression { $$ = CB.exprShiftLeft(UD, merge(@1, @3), $1, $3); }
    | shift_expression T_RSHIFT additive_expression { $$ = CB.exprShiftRight(UD, merge(@1, @3), $1, $3); }
    ;

relational_expression
    : shift_expression { $$ = $1; }
    | relational_expression T_GREATER shift_expression { $$ = CB.exprGreater(UD, merge(@1, @3), $1, $3); }
    | relational_expression T_GREATER_EQ shift_expression { $$ = CB.exprGreaterEq(UD, merge(@1, @3), $1, $3); }
    | relational_expression T_LESS shift_expression { $$ = CB.exprLess(UD, merge(@1, @3), $1, $3); }
    | relational_expression T_LESS_EQ shift_expression { $$ = CB.exprLessEq(UD, merge(@1, @3), $1, $3); }
    ;

equality_expression
    : relational_expression { $$ = $1; }
    | equality_expression T_EQUAL relational_expression { $$ = CB.exprEqual(UD, merge(@1, @3), $1, $3); }
    | equality_expression T_NOT_EQUAL relational_expression { $$ = CB.exprNotEqual(UD, merge(@1, @3), $1, $3); }
    ;

and_expression
    : equality_expression { $$ = $1; }
    | and_expression T_AMPERSAND equality_expression { $$ = CB.exprBitwiseAnd(UD, merge(@1, @3), $1, $3); }
    ;

exclusive_or_expression
    : and_expression { $$ = $1; }
    | exclusive_or_expression T_CARET and_expression { $$ = CB.exprBitwiseXor(UD, merge(@1, @3), $1, $3); }
    ;

inclusive_or_expression
    : exclusive_or_expression { $$ = $1; }
    | inclusive_or_expression T_VBAR exclusive_or_expression { $$ = CB.exprBitwiseOr(UD, merge(@1, @3), $1, $3); }
    ;

logical_and_expression
    : inclusive_or_expression { $$ = $1; }
    | logical_and_expression T_DBLAMPERSAND inclusive_or_expression { $$ = CB.exprLogicAnd(UD, merge(@1, @3), $1, $3); }
    ;

logical_or_expression
    : logical_and_expression { $$ = $1; }
    | logical_or_expression T_DBLVBAR logical_and_expression { $$ = CB.exprLogicOr(UD, merge(@1, @3), $1, $3); }
    ;

conditional_expression
    : logical_or_expression { $$ = $1; }
    | logical_or_expression T_QUESTION expression T_COLON conditional_expression
      { $$ = CB.exprTernary(UD, merge(@1, @5), $1, $3, $5); }
    ;

assignment_expression
    : conditional_expression { $$ = $1; }
    | unary_expression T_ASSIGN assignment_expression { $$ = CB.exprAssign(UD, merge(@1, @3), $1, $3); }
    | unary_expression T_ADD_ASSIGN assignment_expression { $$ = CB.exprAssignAdd(UD, merge(@1, @3), $1, $3); }
    | unary_expression T_SUB_ASSIGN assignment_expression { $$ = CB.exprAssignSub(UD, merge(@1, @3), $1, $3); }
    | unary_expression T_MUL_ASSIGN assignment_expression { $$ = CB.exprAssignMul(UD, merge(@1, @3), $1, $3); }
    | unary_expression T_DIV_ASSIGN assignment_expression { $$ = CB.exprAssignDiv(UD, merge(@1, @3), $1, $3); }
    | unary_expression T_MOD_ASSIGN assignment_expression { $$ = CB.exprAssignMod(UD, merge(@1, @3), $1, $3); }
    | unary_expression T_AND_ASSIGN assignment_expression { $$ = CB.exprAssignAnd(UD, merge(@1, @3), $1, $3); }
    | unary_expression T_OR_ASSIGN assignment_expression { $$ = CB.exprAssignOr(UD, merge(@1, @3), $1, $3); }
    | unary_expression T_XOR_ASSIGN assignment_expression { $$ = CB.exprAssignXor(UD, merge(@1, @3), $1, $3); }
    | unary_expression T_SHL_ASSIGN assignment_expression { $$ = CB.exprAssignShl(UD, merge(@1, @3), $1, $3); }
    | unary_expression T_SHR_ASSIGN assignment_expression { $$ = CB.exprAssignShr(UD, merge(@1, @3), $1, $3); }
    ;

expression
    : assignment_expression { $$ = $1; }
    ;

optional_expression
    : /* empty */ { $$ = NULL; }
    | expression { $$ = $1; }
    ;

/*********************************************************************************************************************/
/* Statements */

statement_list
    : /* empty */
    | statement_list statement
    ;

statement
    : T_SEMICOLON { CB.stmtEmpty(UD, &@1); }
    | expression T_SEMICOLON { CB.stmtExpr(UD, merge(@1, @2), $1); }
    | variable_declaration
    | compound_statement
    | T_IDENTIFIER T_COLON { CB.stmtLabel(UD, merge(@1, @2), $1->text); }
    | KW_goto T_IDENTIFIER T_SEMICOLON { CB.stmtGoto(UD, merge(@1, @3), &@2, $2->text); }
    | KW_break T_SEMICOLON { CB.stmtBreak(UD, merge(@1, @2)); }
    | KW_continue T_SEMICOLON { CB.stmtContinue(UD, merge(@1, @2)); }
    | KW_delete expression T_SEMICOLON { CB.stmtDelete(UD, merge(@1, @3), $2); }
    | KW_throw optional_expression T_SEMICOLON { CB.stmtThrow(UD, merge(@1, @3), $2); }
    | KW_return optional_expression T_SEMICOLON { CB.stmtReturn(UD, merge(@1, @3), $2); }
    | statement_if_start optional_variable_declaration statement_if_then statement optional_else statement_if_end
    | statement_while_start optional_variable_declaration statement_while_do statement statement_while_end
    | statement_do_start statement_do_body statement_do_end
    | statement_do_curly_start statement_do_curly_body statement_do_end
    | statement_for_start statement_for_init statement_for_cond statement_for_update statement_for_do statement statement_for_end
    | statement_switch_start statement_switch_operand statement_switch_body statement_switch_end
    | statement_try_start compound_statement catch_list optional_finally statement_try_end
    ;

compound_statement : statement_compound_start statement_list statement_compound_end;
statement_compound_start : T_LCURLY { CB.stmtCompoundBegin(UD, &@1); };
statement_compound_end : T_RCURLY { CB.stmtCompoundEnd(UD, &@1); };

optional_else : /* empty */ | statement_if_else statement;
statement_if_start : KW_if T_LPAREN { CB.stmtIfBegin(UD, &@1); };
statement_if_then : expression T_RPAREN { CB.stmtIfThen(UD, &@2, $1); };
statement_if_else : KW_else { CB.stmtIfElse(UD, &@1); };
statement_if_end : /* empty */ { CB.stmtIfEnd(UD); };

statement_while_start : KW_while T_LPAREN { CB.stmtWhileBegin(UD, &@1); };
statement_while_do : expression T_RPAREN { CB.stmtWhileDo(UD, &@2, $1); };
statement_while_end : /* empty */ { CB.stmtWhileEnd(UD); };

statement_do_start : KW_do { CB.stmtDoBegin(UD, &@1); };
statement_do_curly_start : T_KWdo_WITH_LCURLY { CB.stmtDoBegin(UD, &@1); };
statement_do_body : statement;
statement_do_curly_body : statement_list T_RCURLY;
statement_do_end : KW_while T_LPAREN expression T_RPAREN T_SEMICOLON { CB.stmtDoEnd(UD, merge(@1, @5), $3); };

statement_for_start : KW_for T_LPAREN { CB.stmtForBegin(UD, &@1); };
statement_for_init : variable_declaration_or_expression { if ($1) CB.stmtForInit(UD, &@1, $1); };
statement_for_cond : optional_expression T_SEMICOLON { if ($1) CB.stmtForCondition(UD, $1); };
statement_for_update : optional_expression { if ($1) CB.stmtForUpdate(UD, $1); };
statement_for_do : T_RPAREN { CB.stmtForDo(UD, &@1); };
statement_for_end : /* empty */ { CB.stmtForEnd(UD); };

statement_switch_start : KW_switch { CB.stmtSwitchBegin(UD, &@1); };
statement_switch_operand : T_LPAREN statement_switch_expr T_RPAREN { CB.stmtSwitchOperand(UD, merge(@1, @3), $2); };
statement_switch_expr : optional_variable_declaration expression { $$ = $2; };
statement_switch_body : T_LCURLY statement_list_switch T_RCURLY;
statement_switch_end : /* empty */ { CB.stmtSwitchEnd(UD); };
statement_list_switch
    : /* empty */
    | statement_list_switch KW_default T_COLON { CB.stmtSwitchDefault(UD, &@2); }
    | statement_list_switch KW_case expression T_COLON { CB.stmtSwitchCase(UD, &@2, $3); }
    | statement_list_switch KW_case expression T_DBLDOT expression T_COLON { CB.stmtSwitchCaseRange(UD, &@2, $3, $5); }
    | statement_list_switch statement
    ;

statement_try_start : KW_try { CB.stmtTryBegin(UD, &@1); };
statement_try_end : /* empty */ { CB.stmtTryEnd(UD); };
catch_list : /* empty */ | catch_list catch_statement;
catch_statement : catch_statement_start optional_catch_clause catch_statement_body;
catch_statement_start : KW_catch { CB.stmtTry_CatchBegin(UD, &@1); };
optional_catch_clause : /* empty */ | T_LPAREN variable_declarator_list T_RPAREN { CB.stmtTry_CatchDo(UD, &@3); };
catch_statement_body : T_LCURLY statement_list T_RCURLY { CB.stmtTry_CatchEnd(UD); };
optional_finally : /* empty */ | statement_finally_begin compound_statement statement_finally_end;
statement_finally_begin : KW_finally { CB.stmtTry_FinallyBegin(UD, &@1); };
statement_finally_end : /* empty */ { CB.stmtTry_FinallyEnd(UD); };

/*********************************************************************************************************************/
/* Struct initializers */

struct_initializer
    : T_LCURLY optional_struct_initializer_member_list T_RCURLY
    ;

optional_struct_initializer_member_list
    : empty_struct_initializer_list { CB.structInitializerBegin(UD); CB.structInitializerEnd(UD); }
    | begin_struct_initializer_member_list struct_initializer_member_list end_struct_initializer_member_list
    | begin_struct_initializer_member_list struct_initializer_member_list T_COMMA end_struct_initializer_member_list
    ;

empty_struct_initializer_list : begin_struct_initializer_member_list end_struct_initializer_member_list;
begin_struct_initializer_member_list : /* empty */ { CB.structInitializerBegin(UD); };
end_struct_initializer_member_list : /* empty */ { CB.structInitializerEnd(UD); };

struct_initializer_member_list
    : struct_initializer_member
    | struct_initializer_member_list T_COMMA struct_initializer_member
    ;

struct_initializer_member
    : T_DOT T_IDENTIFIER T_COLON expression
      { CB.structInitializer(UD, merge(@1, @2), $2->text, $4); }
    ;

/*********************************************************************************************************************/
/* Types */

type_name
    : KW_void { $$ = CB.typeVoid(UD, &@1); }
    | KW_bit { $$ = CB.typeBit(UD, &@1, NULL); }
    | T_KWbit_WITH_LBRACKET expression T_RBRACKET { $$ = CB.typeBit(UD, &@1, $2); }
    | KW_bool { $$ = CB.typeBool(UD, &@1); }
    | KW_sbyte { $$ = CB.typeInt8(UD, &@1); }
    | KW_byte { $$ = CB.typeUInt8(UD, &@1); }
    | KW_int8 { $$ = CB.typeInt8(UD, &@1); }
    | KW_uint8 { $$ = CB.typeUInt8(UD, &@1); }
    | KW_int { $$ = CB.typeInt16(UD, &@1); }
    | KW_word { $$ = CB.typeUInt16(UD, &@1); }
    | KW_int16 { $$ = CB.typeInt16(UD, &@1); }
    | KW_uint16 { $$ = CB.typeUInt16(UD, &@1); }
    | KW_int32 { $$ = CB.typeInt32(UD, &@1); }
    | KW_uint32 { $$ = CB.typeUInt32(UD, &@1); }
    | KW_object { $$ = CB.typeObject(UD, &@1); }
    | T_IDENTIFIER { $$ = CB.typeIdentifier(UD, &@1, $1->text); }
    | type_name T_ASTERISK { $$ = CB.typePointer(UD, merge(@1, @2), $1); }
    ;

/*********************************************************************************************************************/
/* Attributes */

optional_attribute_list : /* empty */ | attribute_list_start attribute_list attribute_list_end;
attribute_list_start : /* empty */ { CB.attrListBegin(UD); };
attribute_list : attribute | attribute_list attribute;
attribute_list_end : /* empty */ { CB.attrListEnd(UD); };
attribute : attribute_start optional_attribute_param_list attribute_end;
attribute_start : T_ATSIGN T_IDENTIFIER { CB.attrBegin(UD, merge(@1, @2), $2->text); };
attribute_end : /* empty */ { CB.attrEnd(UD); };
optional_attribute_param_list : /* empty */ | T_LPAREN attribute_param_list T_RPAREN;
attribute_param_list : attribute_param | attribute_param_list T_COMMA attribute_param;
attribute_param
    : expression { CB.attrParam(UD, NULL, NULL, $1); }
    | T_IDENTIFIER T_COLON expression { CB.attrParam(UD, &@1, $1->text, $3); }
    ;

/*********************************************************************************************************************/
/* Variable declarations */

variable_declaration
    : optional_attribute_list variable_decl_start variable_declarator_list variable_decl_end
    ;

optional_variable_declaration
    : /* empty */ | variable_declaration
    ;

variable_decl_start : optional_static var_or_const { CB.varDeclBegin(UD, &@2, ($1 ? &@1 : NULL), $2); };
variable_decl_end : T_SEMICOLON { CB.varDeclEnd(UD, &@1); };
var_or_const : KW_var { $$ = false; } | KW_const { $$ = true; };
variable_declarator_list : variable_declarator | variable_declarator_list T_COMMA variable_declarator;

variable_declarator
    : variable_declarator_start optional_initializer variable_end
    | variable_array_declarator_start T_ASSIGN array_initializer variable_end
    ;

variable_declarator_start
    : T_IDENTIFIER
      { CB.varBegin(UD, &@1, $1->text); }
    | T_LPAREN type_name T_RPAREN T_IDENTIFIER
      { CB.varBegin(UD, &@4, $4->text); CB.varType(UD, &@2, $2); }
    ;

variable_array_declarator_start
    : T_LPAREN type_name T_LBRACKET optional_expression T_RBRACKET T_RPAREN T_IDENTIFIER
      { CB.varBegin(UD, &@7, $7->text); CB.varType_Array(UD, merge(@3, @5), $2, $4); }
    ;

variable_end : /* empty */ { CB.varEnd(UD); }

optional_initializer
    : /* empty */
    | T_ASSIGN struct_initializer
    | T_ASSIGN expression { CB.varInitializer(UD, $2); }
    ;

array_initializer : array_initializer_start optional_array_initializer_list array_initializer_end;
array_initializer_start : T_LCURLY { CB.arrayInitializerBegin(UD, &@1); };
array_initializer_end : T_RCURLY { CB.arrayInitializerEnd(UD, &@1); };
optional_array_initializer_list : /* empty */ | array_initializer_list | array_initializer_list T_COMMA;
array_initializer_list : array_element_initializer | array_initializer_list T_COMMA array_element_initializer;
array_element_initializer : expression { CB.arrayInitializer(UD, $1); };

variable_declaration_or_expression
    : T_SEMICOLON { $$ = NULL; }
    | expression T_SEMICOLON { $$ = $1; }
    | variable_declaration { $$ = NULL; }
    ;

/*********************************************************************************************************************/
/* Class declarations */

class_declaration
    : optional_attribute_list class_declaration_start optional_parent_class_list class_body
    | optional_attribute_list interface_declaration_start optional_parent_class_list class_body
    ;

class_declaration_start : KW_class T_IDENTIFIER optional_final {
        if (BUNDLE)
            compilerBundleAddClass(BUNDLE, $2->text);
        CB.classBegin(UD, &@1, &@2, $2->text, $3);
    };

interface_declaration_start : KW_interface T_IDENTIFIER { CB.classBeginInterface(UD, &@1, &@2, $2->text); };
optional_final : /* empty */ { $$ = false; } | KW_final { $$ = true; };
optional_parent_class_list : /* empty */ | T_COLON parent_class_list;
parent_class_list : parent_class_name | parent_class_list T_COMMA parent_class_name;
parent_class_name : T_IDENTIFIER { CB.classParent(UD, &@1, $1->text); };
class_body_start : T_LCURLY { CB.classMembersBegin(UD, &@1); };
class_body_end : T_RCURLY { CB.classMembersEnd(UD, &@1); CB.classEnd(UD, &@1); };
class_body : class_body_start class_member_list class_body_end;
class_member_list : /* empty */ | class_member_list class_member;

class_member
    : optional_attribute_list class_variables
    | optional_attribute_list class_destructor
    | optional_attribute_list class_method
    | KW_friend KW_class T_IDENTIFIER T_SEMICOLON { CB.classFriend(UD, &@1, &@3, $3->text); }
    ;

class_member_visibility
    : KW_public { $$ = COMPILER_PUBLIC; }
    | KW_protected { $$ = COMPILER_PROTECTED; }
    | KW_private { $$ = COMPILER_PRIVATE; }
    ;

optional_static : /* empty */ { $$ = false; } | KW_static { $$ = true; };

class_variables : class_variables_start variable_declarator_list class_variables_end;
class_variables_start : class_member_visibility optional_static var_or_const
                            { CB.classVariablesBegin(UD, &@3, &@1, $1, ($2 ? &@2 : NULL), $3); };
class_variables_end : T_SEMICOLON { CB.classVariablesEnd(UD, &@1); };

class_destructor : class_destructor_start compound_statement class_destructor_end;
class_destructor_start : class_member_visibility T_TILDE T_IDENTIFIER
                            { CB.classDestructorBegin(UD, merge(@2, @3), $2->text, &@1, $1); };
class_destructor_end : /* empty */ { CB.classDestructorEnd(UD); };

class_method : class_method_start method_name method_name_end method_body;
class_method_start : class_member_visibility optional_static T_LPAREN type_name T_RPAREN
                            { CB.classMethodBegin(UD, merge(@3, @5), &@1, $1, ($2 ? &@2 : NULL), &@4, $4); };
method_name : method_name_simple | method_name_with_args;
method_name_simple : T_IDENTIFIER { CB.classMethodNameSimple(UD, &@1, $1->text); };
method_name_with_args : method_arg | method_name_with_args method_arg;
method_arg : T_IDENTIFIER T_COLON T_LPAREN type_name T_RPAREN T_IDENTIFIER
                            { CB.classMethodNameArg(UD, merge(@1, @2), $1->text, $4, &@6, $6->text); };

method_body : optional_compound_statement;
method_name_end : optional_final optional_override { CB.classMethodNameEnd(UD, ($1 ? &@1 : NULL), ($2 ? &@2 : NULL)); };
optional_override : /* empty */ { $$ = false; } | KW_override { $$ = true; };
method_body_start : /* empty */ { CB.classMethodBodyBegin(UD); };
method_body_end : /* empty */ { CB.classMethodEnd(UD); };
optional_compound_statement
    : method_body_start compound_statement method_body_end
    | KW_extern T_SEMICOLON { CB.classMethodEnd_Extern(UD, &@1); };
    | KW_abstract T_SEMICOLON { CB.classMethodEnd_Abstract(UD, &@1); };
    ;

/*********************************************************************************************************************/
/* Enum declarations */

enum_declaration : optional_attribute_list enum_declaration_start enum_optional_type_name enum_members;

enum_declaration_start : enum_or_flags T_IDENTIFIER { CB.enumBegin(UD, &@1, &@2, $2->text, $1); };
enum_or_flags : KW_enum { $$ = false; } | KW_flags { $$ = true; };
enum_optional_type_name : /* empty */ | T_COLON type_name { CB.enumType(UD, &@2, $2); };

enum_members: enum_members_start optional_enum_member_list enum_members_end;
enum_members_start : T_LCURLY { CB.enumMembersBegin(UD, &@1); };
enum_members_end : T_RCURLY { CB.enumMembersEnd(UD, &@1); CB.enumEnd(UD, &@1); };

optional_enum_member_list : /* empty */ | enum_member_list | enum_member_list T_COMMA;
enum_member_list : enum_member | enum_member_list T_COMMA enum_member;

enum_member
    : T_IDENTIFIER { CB.enumMember(UD, &@1, $1->text, NULL); }
    | T_IDENTIFIER T_ASSIGN expression { CB.enumMember(UD, &@1, $1->text, $3); }
    ;

/*********************************************************************************************************************/
/* Struct declarations */

struct_declaration : optional_attribute_list struct_declaration_start struct_optional_parent struct_members;

struct_declaration_start : struct_or_union T_IDENTIFIER { CB.structBegin(UD, &@1, &@2, $2->text, $1); }
struct_or_union : KW_struct { $$ = false; } | KW_union { $$ = true; };
struct_optional_parent : /* empty */ | T_COLON T_IDENTIFIER { CB.structParent(UD, &@2, $2->text); };

struct_members: struct_members_start struct_member_list struct_members_end;
struct_members_start : T_LCURLY { CB.structMembersBegin(UD, &@1); };
struct_members_end : T_RCURLY { CB.structMembersEnd(UD, &@1); CB.structEnd(UD, &@1); };

struct_member_list : /* empty */ | struct_member_list struct_member_with_attributes;
struct_member_with_attributes : optional_attribute_list struct_member;
struct_member : T_IDENTIFIER T_COLON type_name T_SEMICOLON { CB.structMember(UD, &@1, $1->text, $3); };

/*********************************************************************************************************************/

translation_unit : translation_unit_start translation_unit_contents translation_unit_end;
translation_unit_start : /* empty */ { CB.translationUnitBegin(UD); }
translation_unit_end : /* empty */ { CB.translationUnitEnd(UD); }

translation_unit_contents
    : /* empty */
    | translation_unit_contents class_declaration
    | translation_unit_contents enum_declaration
    | translation_unit_contents struct_declaration
    ;

/*********************************************************************************************************************/

%%

#ifdef BORLAND
#pragma option -wpia
#pragma option -waus
#pragma option -wrch
#endif

/*#undef compiler*/
#undef yylval
#undef yyposn

STRUCT(ChainedToken)
{
    ChainedToken* next;
    CompilerToken token;
};

static ChainedToken* firstToken = NULL;
static ChainedToken* lastToken = NULL;

void compilerBeginParse(Compiler* compiler)
{
    UNUSED(compiler);
    firstToken = NULL;
    lastToken = NULL;
}

void compilerPushToken(Compiler* compiler)
{
    compilerPushTokenEx(compiler, &compiler->lexer.token);
}

void compilerPushTokenEx(Compiler* compiler, const CompilerToken* token)
{
    ChainedToken* chained = (ChainedToken*)compilerTempAlloc(compiler, sizeof(ChainedToken));
    chained->next = NULL;
    memcpy(&chained->token, token, sizeof(CompilerToken));
    if (!firstToken)
        firstToken = chained;
    else
        lastToken->next = chained;
    lastToken = chained;
}

void compilerPushTokenEof(Compiler* compiler, SourceLine* line, int column)
{
    compiler->lexer.token.id = T_EOF;
    compiler->lexer.token.name = "<end of file>";
    compiler->lexer.token.location.startLine = line;
    compiler->lexer.token.location.endLine = line;
    compiler->lexer.token.location.startColumn = column;
    compiler->lexer.token.location.endColumn = column;
    compilerPushToken(compiler);
}

void compilerEndParse(Compiler* compiler)
{
    yycontext yyctx;

    memset(&yyctx, 0, sizeof(yyctx));
    yyctx.userdata = &compiler->parser;

    yyparse(&yyctx);
}

void yyreduceposn(Compiler* compiler, CompilerLocation* ret, const CompilerLocation* inputs, int count)
{
    int start = 0, end = count - 1;

    for (; end >= 0; --end) {
        if (inputs[end].file)
            break;
    }
    for (; start < end; ++start) {
        if (inputs[start].file)
            break;
    }

    if (start > end)
        return;

    if (start == end) {
        *ret = inputs[start];
        return;
    }

    compilerMergeLocationsInto(compiler, ret, &inputs[start], &inputs[end]);
}

void yyerror(yycontext* ctx, int yychar, const YYSTYPE* yylval, const YYPOSN* yyposn)
{
    CompilerParser* parser = (CompilerParser*)ctx->userdata;
    UNUSED(yychar);
    parser->cb.error(parser->cb.ud, yyposn, yylval->token);
}

static ChainedToken* nextToken(ChainedToken** curToken)
{
    ChainedToken* token;
    int id;

    do {
        token = *curToken;
        id = token->token.id;
        if (id == T_EOF)
            break;
        *curToken = token->next;
    } while (id == T_SINGLE_LINE_COMMENT || id == T_MULTI_LINE_COMMENT);

    return token;
}

int yylex(yycontext* ctx)
{
    int id;

    const ChainedToken* token = nextToken(&firstToken);

    id = token->token.id;
    if (id == KW_do) {
        ChainedToken* ptr = firstToken;
        ChainedToken* next = nextToken(&ptr);
        if (next->token.id == T_LCURLY) {
            id = T_KWdo_WITH_LCURLY;
            firstToken = next->next;
        }
    } else if (id == KW_bit) {
        ChainedToken* ptr = firstToken;
        ChainedToken* next = nextToken(&ptr);
        if (next->token.id == T_LBRACKET) {
            id = T_KWbit_WITH_LBRACKET;
            firstToken = next->next;
        }
    }

    ctx->yylval.token = (CompilerToken*)&token->token;
    ctx->yyposn = token->token.location;

    return id;
}
