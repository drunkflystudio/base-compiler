#include <drunkfly/compiler/lexer.h>
#include <drunkfly/compiler/arena.h>
#include <assert.h>
#include <lauxlib.h>

enum LexerState
{
    NORMAL,
    MULTILINE_COMMENT
};

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

void compilerInitLexer(CompilerLexer* lexer, SourceFile* file, SourceLine* line, const char* text, int state)
{
    lexer->token.location.file = file;
    lexer->token.location.startLine = line;
    lexer->token.location.endLine = line;
    lexer->start = text;
    lexer->cursor = text;
    lexer->state = state;
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

bool compilerGetToken(Compiler* compiler, lua_State* L, CompilerLexer* lexer)
{
    const char* tokenStart;
    bool emitComment;

    #define EMIT(NAME, TEXT) \
        (lexer->token.id = (short)T_##NAME, \
         lexer->token.location.endColumn = (int)(lexer->cursor - lexer->start), \
         lexer->token.name = "'" TEXT "'", \
         true)
    #define EMIT_SPECIAL(NAME, TEXT) \
        (lexer->token.id = (short)T_##NAME, \
         lexer->token.location.endColumn = (int)(lexer->cursor - lexer->start), \
         lexer->token.name = TEXT, \
         true)
    #define EMIT_KEYWORD(NAME) \
        (lexer->token.id = (short)KW_##NAME, \
         lexer->token.location.endColumn = (int)(lexer->cursor - lexer->start), \
         lexer->token.name = "'" #NAME "'", \
         true)

    switch (lexer->state) {
        default:
            assert(false);
            luaL_error(L, "invalid lexer state.");
            return false;

        case MULTILINE_COMMENT:
            emitComment = (*lexer->cursor != 0);
            lexer->token.integer = 0;
            lexer->token.text = NULL;
            lexer->token.location.startColumn = (int)(lexer->cursor - lexer->start);
          multiline_comment:
            for (;;) {
                /* End of line or file */
                if (*lexer->cursor == 0) {
                    if (emitComment)
                        return EMIT_SPECIAL(MULTI_LINE_COMMENT, "comment");
                    return false;
                }

                /* End of comment */
                if (lexer->cursor[0] == '*' && lexer->cursor[1] == '/') {
                    lexer->cursor += 2;
                    lexer->state = NORMAL;
                    return EMIT_SPECIAL(MULTI_LINE_COMMENT, "comment");
                }

                ++lexer->cursor;
            }

        case NORMAL:
        normal:
            lexer->token.integer = 0;
            lexer->token.text = NULL;
            lexer->token.location.startColumn = (int)(lexer->cursor - lexer->start);
            lexer->token.overflow = false;
            tokenStart = lexer->cursor;

            /*!re2c

            re2c:yyfill:enable = 0;
            re2c:define:YYCURSOR = lexer->cursor;
            re2c:define:YYCTYPE = char;

            "\x00"                      { return false; }
            [ \t\v\f]                   { goto normal; }

            "//"                        { while (*lexer->cursor)
                                              ++lexer->cursor;
                                          return EMIT_SPECIAL(SINGLE_LINE_COMMENT, "comment");
                                        }
            "/*"                        { lexer->state = MULTILINE_COMMENT;
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

            [a-zA-Z_][a-zA-Z0-9_]*      { size_t len = (size_t)(lexer->cursor - tokenStart);
                                          lexer->token.text = compilerTempDupStrN(compiler, tokenStart, len);
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
                                          } while (p != lexer->cursor);
                                          if (overflow)
                                              lexer->token.overflow = true;
                                          if (error)
                                              return EMIT_SPECIAL(INVALID_BINARY_LITERAL, "binary constant");
                                          lexer->token.integer = value;
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
                                          } while (p != lexer->cursor);
                                          if (overflow)
                                              lexer->token.overflow = true;
                                          if (error)
                                              return EMIT_SPECIAL(INVALID_OCTAL_LITERAL, "octal constant");
                                          lexer->token.integer = value;
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
                                          } while (p != lexer->cursor);
                                          if (overflow)
                                              lexer->token.overflow = true;
                                          if (error)
                                              return EMIT_SPECIAL(INVALID_HEXADECIMAL_LITERAL, "hexadecimal constant");
                                          lexer->token.integer = value;
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
                                          } while (p != lexer->cursor);
                                          if (overflow)
                                              lexer->token.overflow = true;
                                          if (error)
                                              return EMIT_SPECIAL(INVALID_DECIMAL_LITERAL, "decimal constant");
                                          lexer->token.integer = value;
                                          return EMIT_SPECIAL(INTEGER_LITERAL, "decimal constant");
                                        }

            *                           { return EMIT_SPECIAL(INVALID_SYMBOL, "invalid symbol"); }

           */
    }
}
