#include <drunkfly/compiler/private.h>
#include <drunkfly/compiler/arena.h>
#include <assert.h>
#include <lauxlib.h>

static unsigned int digitToNumber(char ch, bool* error)
{
    switch (ch) {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'A': case 'a': return 10;
        case 'B': case 'b': return 11;
        case 'C': case 'c': return 12;
        case 'D': case 'd': return 13;
        case 'E': case 'e': return 14;
        case 'F': case 'f': return 15;
    }
    *error = true;
    return 0;
}

bool compilerReadLine(const char** ptr, const char** outLine, size_t* outLength)
{
    const char* start = *ptr;
    const char* end = start;
    const char* next = start;
    bool result;

    *outLine = end;

    for (;;) {
        if (!*end) {
            result = false;
            break;
        }
        if (*end == '\r') {
            next = end + 1;
            if (*next == '\n')
                ++next;
            result = true;
            break;
        }
        if (*end == '\n') {
            next = end + 1;
            if (*next == '\r')
                ++next;
            result = true;
            break;
        }
        ++end;
    }

    *outLength = (size_t)(end - start);
    *ptr = next;
    return result;
}

#define END_MARKER ((uint32_t)(0xfffffffful))
static void compilerReadChar(Compiler* compiler)
{
    compiler->lexer.prevCursor = compiler->lexer.cursor;
    if (compiler->lexer.cursor >= compiler->lexer.end) {
        if (compiler->lexer.curChar != END_MARKER) {
            ++compiler->lexer.column;
            compiler->lexer.curChar = END_MARKER;
        }
    } else {
        /* FIXME: implement UTF-8 */
        compiler->lexer.curChar = (uint32_t)*compiler->lexer.cursor++;
        ++compiler->lexer.column;
    }
}

void compilerBeginLine(Compiler* compiler, SourceFile* file, SourceLine* line, const char* text, size_t len, int state)
{
    compiler->lexer.token.location.file = file;
    compiler->lexer.token.location.startLine = line;
    compiler->lexer.token.location.endLine = line;
    compiler->lexer.end = text + len;
    compiler->lexer.cursor = text;
    compiler->lexer.prevCursor = text;
    compiler->lexer.curChar = 0;
    compiler->lexer.column = 0;
    compiler->lexer.state = state;
    compilerReadChar(compiler);
}

#if defined(__GNUC__) && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2))
# if defined(__clang__) || (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#  pragma GCC diagnostic ignored "-Wpedantic"
#  pragma GCC diagnostic ignored "-Wchar-subscripts"
# else
#  pragma GCC diagnostic ignored "-pedantic"
#  pragma GCC diagnostic ignored "-Wchar-subscripts"
# endif
#endif

bool compilerGetToken(Compiler* compiler)
{
    const char* tokenStart;
    bool emitComment;

    #define EMIT(NAME, TEXT) \
        (compiler->lexer.token.id = (short)T_##NAME, \
         compiler->lexer.token.location.endColumn = compiler->lexer.column - 1, \
         compiler->lexer.token.name = "'" TEXT "'", \
         true)
    #define EMIT_SPECIAL(NAME, TEXT) \
        (compiler->lexer.token.id = (short)T_##NAME, \
         compiler->lexer.token.location.endColumn = compiler->lexer.column - 1, \
         compiler->lexer.token.name = TEXT, \
         true)
    #define EMIT_KEYWORD(NAME) \
        (compiler->lexer.token.id = (short)KW_##NAME, \
         compiler->lexer.token.location.endColumn = compiler->lexer.column - 1, \
         compiler->lexer.token.name = "'" #NAME "'", \
         true)

    switch (compiler->lexer.state) {
        default:
            assert(false);
            return (bool)luaL_error(compiler->L, "invalid lexer state.");

        case LEXER_MULTILINE_COMMENT:
            emitComment = (compiler->lexer.curChar != END_MARKER);
            compiler->lexer.token.integer = 0;
            compiler->lexer.token.text = NULL;
            compiler->lexer.token.location.startColumn = compiler->lexer.column;
          multiline_comment:
            for (;;) {
                uint32_t ch = compiler->lexer.curChar;

                /* End of line or file */
                if (ch == END_MARKER) {
                  unterminated_comment:
                    if (emitComment)
                        return EMIT_SPECIAL(MULTI_LINE_COMMENT, "multi-line comment");
                    return false;
                }

                compilerReadChar(compiler);

                /* End of comment */
                if (ch == '*') {
                    ch = compiler->lexer.curChar;
                    if (ch == END_MARKER)
                        goto unterminated_comment;
                    if (ch == '/') {
                        compilerReadChar(compiler);
                        compiler->lexer.state = LEXER_NORMAL;
                        return EMIT_SPECIAL(MULTI_LINE_COMMENT, "multi-line comment");
                    }
                }
            }

        case LEXER_NORMAL:
        normal:
            compiler->lexer.token.integer = 0;
            compiler->lexer.token.text = NULL;
            compiler->lexer.token.location.startColumn = compiler->lexer.column;
            compiler->lexer.token.overflow = false;
            tokenStart = compiler->lexer.prevCursor;

            if (compiler->lexer.curChar == END_MARKER)
                return false;

            /*!re2c

            re2c:api = custom;
            re2c:api:style = free-form;
            re2c:encoding:utf32 = 1;
            re2c:yyfill:enable = 0;
            re2c:define:YYCTYPE = uint32_t;
            re2c:define:YYPEEK = compiler->lexer.curChar;
            re2c:define:YYSKIP = "compilerReadChar(compiler);";

            [ \t\v\f\r\n]               { goto normal; }

            "//"                        { while (compiler->lexer.curChar != END_MARKER)
                                              compilerReadChar(compiler);
                                          return EMIT_SPECIAL(SINGLE_LINE_COMMENT, "single line comment");
                                        }
            "/*"                        { compiler->lexer.state = LEXER_MULTILINE_COMMENT;
                                          emitComment = true;
                                          goto multiline_comment;
                                        }

            "abstract"                  { return EMIT_KEYWORD(abstract); }
            "bit"                       { return EMIT_KEYWORD(bit); }
            "bool"                      { return EMIT_KEYWORD(bool); }
            "break"                     { return EMIT_KEYWORD(break); }
            "byte"                      { return EMIT_KEYWORD(byte); }
            "case"                      { return EMIT_KEYWORD(case); }
            "catch"                     { return EMIT_KEYWORD(catch); }
            "class"                     { return EMIT_KEYWORD(class); }
            "const"                     { return EMIT_KEYWORD(const); }
            "continue"                  { return EMIT_KEYWORD(continue); }
            "default"                   { return EMIT_KEYWORD(default); }
            "delete"                    { return EMIT_KEYWORD(delete); }
            "do"                        { return EMIT_KEYWORD(do); }
            "else"                      { return EMIT_KEYWORD(else); }
            "enum"                      { return EMIT_KEYWORD(enum); }
            "false"                     { return EMIT_KEYWORD(false); }
            "final"                     { return EMIT_KEYWORD(final); }
            "finally"                   { return EMIT_KEYWORD(finally); }
            "flags"                     { return EMIT_KEYWORD(flags); }
            "for"                       { return EMIT_KEYWORD(for); }
            "friend"                    { return EMIT_KEYWORD(friend); }
            "goto"                      { return EMIT_KEYWORD(goto); }
            "if"                        { return EMIT_KEYWORD(if); }
            "int"                       { return EMIT_KEYWORD(int); }
            "int8"                      { return EMIT_KEYWORD(int8); }
            "int16"                     { return EMIT_KEYWORD(int16); }
            "int32"                     { return EMIT_KEYWORD(int32); }
            "interface"                 { return EMIT_KEYWORD(interface); }
            "new"                       { return EMIT_KEYWORD(new); }
            "null"                      { return EMIT_KEYWORD(null); }
            "object"                    { return EMIT_KEYWORD(object); }
            "override"                  { return EMIT_KEYWORD(override); }
            "private"                   { return EMIT_KEYWORD(private); }
            "protected"                 { return EMIT_KEYWORD(protected); }
            "public"                    { return EMIT_KEYWORD(public); }
            "sbyte"                     { return EMIT_KEYWORD(sbyte); }
            "sizeof"                    { return EMIT_KEYWORD(sizeof); }
            "static"                    { return EMIT_KEYWORD(static); }
            "struct"                    { return EMIT_KEYWORD(struct); }
            "switch"                    { return EMIT_KEYWORD(switch); }
            "throw"                     { return EMIT_KEYWORD(throw); }
            "true"                      { return EMIT_KEYWORD(true); }
            "try"                       { return EMIT_KEYWORD(try); }
            "uint8"                     { return EMIT_KEYWORD(uint8); }
            "uint16"                    { return EMIT_KEYWORD(uint16); }
            "uint32"                    { return EMIT_KEYWORD(uint32); }
            "union"                     { return EMIT_KEYWORD(union); }
            "var"                       { return EMIT_KEYWORD(var); }
            "void"                      { return EMIT_KEYWORD(void); }
            "while"                     { return EMIT_KEYWORD(while); }
            "word"                      { return EMIT_KEYWORD(word); }

            "."                         { return EMIT(DOT, "."); }
            ".."                        { return EMIT(DOUBLE_DOT, ".."); }
            "["                         { return EMIT(LBRACKET, "["); }
            "]"                         { return EMIT(RBRACKET, "]"); }
            "++"                        { return EMIT(INCR, "++"); }
            "--"                        { return EMIT(DECR, "--"); }
            ";"                         { return EMIT(SEMICOLON, ";"); }
            "&"                         { return EMIT(AMPERSAND, "&"); }
            "*"                         { return EMIT(ASTERISK, "*"); }
            "+"                         { return EMIT(PLUS, "+"); }
            "-"                         { return EMIT(MINUS, "-"); }
            "~"                         { return EMIT(TILDE, "~"); }
            "!"                         { return EMIT(EXCLAMATION, "!"); }
            "("                         { return EMIT(LPAREN, "("); }
            ")"                         { return EMIT(RPAREN, ")"); }
            "/"                         { return EMIT(SLASH, "/"); }
            "%"                         { return EMIT(PERCENT, "%"); }
            "<<"                        { return EMIT(LSHIFT, "<<"); }
            ">>"                        { return EMIT(RSHIFT, ">>"); }
            "<"                         { return EMIT(LESS, "<"); }
            ">"                         { return EMIT(GREATER, ">"); }
            "<="                        { return EMIT(LESS_EQ, "<="); }
            ">="                        { return EMIT(GREATER_EQ, ">="); }
            "=="                        { return EMIT(EQUAL, "=="); }
            "!="                        { return EMIT(NOT_EQUAL, "!="); }
            "^"                         { return EMIT(CARET, "^"); }
            "|"                         { return EMIT(VBAR, "|"); }
            "&&"                        { return EMIT(DOUBLE_AMPERSAND, "&&"); }
            "||"                        { return EMIT(DOUBLE_VBAR, "||"); }
            "?"                         { return EMIT(QUESTION, "?"); }
            ":"                         { return EMIT(COLON, ":"); }
            "="                         { return EMIT(ASSIGN, "="); }
            "+="                        { return EMIT(ADD_ASSIGN, "+="); }
            "-="                        { return EMIT(SUB_ASSIGN, "-="); }
            "*="                        { return EMIT(MUL_ASSIGN, "*="); }
            "/="                        { return EMIT(DIV_ASSIGN, "/="); }
            "%="                        { return EMIT(MOD_ASSIGN, "%="); }
            "|="                        { return EMIT(OR_ASSIGN, "|="); }
            "&="                        { return EMIT(AND_ASSIGN, "&="); }
            "^="                        { return EMIT(XOR_ASSIGN, "^="); }
            "<<="                       { return EMIT(SHL_ASSIGN, "<<="); }
            ">>="                       { return EMIT(SHR_ASSIGN, ">>="); }
            "{"                         { return EMIT(LCURLY, "{"); }
            "}"                         { return EMIT(RCURLY, "}"); }
            ","                         { return EMIT(COMMA, ","); }
            "@"                         { return EMIT(ATSIGN, "@"); }

            [a-zA-Z_][a-zA-Z0-9_]*      { size_t len = (size_t)(compiler->lexer.cursor - tokenStart);
                                          compiler->lexer.token.text = compilerTempDupStrN(compiler, tokenStart, len);
                                          return EMIT_SPECIAL(IDENTIFIER, "identifier");
                                        }

            "0b" [0-9a-zA-Z_]+          { uint_value_t value = 0, old;
                                          const char* p = tokenStart + 2;
                                          bool error = false, overflow = false;
                                          do {
                                              unsigned int digit = digitToNumber(*p++, &error);
                                              if (digit > 1)
                                                  error = true;
                                              old = value;
                                              value <<= 1;
                                              if (value < old)
                                                  overflow = true;
                                              value |= digit;
                                          } while (p != compiler->lexer.cursor);
                                          if (overflow)
                                              compiler->lexer.token.overflow = true;
                                          if (error)
                                              return EMIT_SPECIAL(INVALID_BINARY_LITERAL, "invalid binary constant");
                                          compiler->lexer.token.integer = value;
                                          return EMIT_SPECIAL(INTEGER_LITERAL, "binary constant");
                                        }

            "0o" [0-9a-zA-Z_]+          { uint_value_t value = 0, old;
                                          const char* p = tokenStart + 2;
                                          bool error = false, overflow = false;
                                          do {
                                              unsigned int digit = digitToNumber(*p++, &error);
                                              if (digit > 7)
                                                  error = true;
                                              old = value;
                                              value <<= 3;
                                              if (value < old)
                                                  overflow = true;
                                              value |= digit;
                                          } while (p != compiler->lexer.cursor);
                                          if (overflow)
                                              compiler->lexer.token.overflow = true;
                                          if (error)
                                              return EMIT_SPECIAL(INVALID_OCTAL_LITERAL, "invalid octal constant");
                                          compiler->lexer.token.integer = value;
                                          return EMIT_SPECIAL(INTEGER_LITERAL, "octal constant");
                                        }

            "0x" [0-9a-zA-Z_]+          { uint_value_t value = 0, old;
                                          const char* p = tokenStart + 2;
                                          bool error = false, overflow = false;
                                          do {
                                              old = value;
                                              value <<= 4;
                                              if (value < old)
                                                  overflow = true;
                                              value |= digitToNumber(*p++, &error);
                                          } while (p != compiler->lexer.cursor);
                                          if (overflow)
                                              compiler->lexer.token.overflow = true;
                                          if (error) {
                                              return EMIT_SPECIAL(
                                                  INVALID_HEXADECIMAL_LITERAL, "invalid hexadecimal constant");
                                          }
                                          compiler->lexer.token.integer = value;
                                          return EMIT_SPECIAL(INTEGER_LITERAL, "hexadecimal constant");
                                        }

            [0-9][0-9a-zA-Z_]*          { uint_value_t value = 0, old;
                                          const char* p = tokenStart;
                                          bool error = false, overflow = false;
                                          do {
                                              unsigned int digit = digitToNumber(*p++, &error);
                                              if (digit > 9)
                                                  error = true;
                                              old = value;
                                              value *= 10;
                                              if (value < old)
                                                  overflow = true;
                                              value += digit;
                                          } while (p != compiler->lexer.cursor);
                                          if (overflow)
                                              compiler->lexer.token.overflow = true;
                                          if (error) {
                                              return EMIT_SPECIAL(
                                                  INVALID_DECIMAL_LITERAL, "invalid decimal constant");
                                          }
                                          compiler->lexer.token.integer = value;
                                          return EMIT_SPECIAL(INTEGER_LITERAL, "decimal constant");
                                        }

            *                           { return EMIT_SPECIAL(INVALID_SYMBOL, "invalid symbol"); }

            */
    }
}
