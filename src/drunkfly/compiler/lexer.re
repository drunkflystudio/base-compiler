#include <drunkfly/compiler/private.h>
#include <drunkfly/compiler/token.h>

STRUCT(Lexer) {
    Compiler* compiler;
    CompilerToken* firstToken;
    CompilerToken* lastToken;
    const char* tokenStart;
    const char* cursor;
    const char* marker;
    const char* fileName;
    int startLine;
    int startColumn;
    int curLine;
    int curColumn;
};

static CompilerToken g_dummyToken;

static void getLocation(Lexer* lexer, CompilerLocation* loc)
{
    /* FIXME loc->fileName = lexer->fileName; */
    loc->startLine = lexer->startLine;
    loc->startColumn = lexer->startColumn;
    loc->endLine = lexer->curLine;
    loc->endColumn = lexer->curColumn - 1;
}

static CompilerToken* emitToken(Lexer* lexer, int tokenID, const char* name)
{
    CompilerToken* token = (CompilerToken*)compilerTempAlloc(lexer->compiler, sizeof(CompilerToken));
    if (!token)
        return &g_dummyToken;

    token->next = NULL;
    token->text = NULL;
    token->name = name;
    token->id = tokenID;
    token->integer = 0;
    getLocation(lexer, &token->location);

    if (lexer->lastToken)
        lexer->lastToken->next = token;
    else
        lexer->firstToken = token;
    lexer->lastToken = token;

    return token;
}

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

CompilerToken* compilerLexer(Compiler* compiler, const char* fileName, const char* fileContents)
{
    CompilerLocation loc;
    Lexer lexer;
    char ch;

    lexer.compiler = compiler;
    lexer.firstToken = NULL;
    lexer.lastToken = NULL;
    lexer.cursor = fileContents;
    lexer.fileName = fileName;
    lexer.startLine = 1;
    lexer.startColumn = 0;
    lexer.curLine = 1;
    lexer.curColumn = 1;

    #define EMIT(NAME, TEXT) \
        lexer.curColumn += (int)sizeof(TEXT) - 1; \
        emitToken(&lexer, T_##NAME, "'" TEXT "'");
    #define EMIT_KEYWORD(NAME) \
        lexer.curColumn += (int)sizeof(#NAME) - 1; \
        emitToken(&lexer, KW_##NAME, "'" #NAME "'");

    lexer.tokenStart = lexer.cursor;
    emitToken(&lexer, T_FILE_START, "file start");

    for (;;) {
        lexer.tokenStart = lexer.cursor;
        lexer.startLine = lexer.curLine;
        lexer.startColumn = lexer.curColumn;

        #ifdef __GNUC__
        #pragma GCC diagnostic ignored "-Wpedantic"
        #pragma GCC diagnostic ignored "-Wchar-subscripts"
        #endif

        /*!re2c

        re2c:yyfill:enable = 0;
        re2c:define:YYCURSOR = lexer.cursor;
        re2c:define:YYMARKER = lexer.marker;
        re2c:define:YYCTYPE = char;

        "\x00"                      { ++lexer.curColumn;
                                      emitToken(&lexer, T_EOF, "<end of file>");
                                      return lexer.firstToken;
                                    }

        "\n" | "\r\n" | "\r"        { ++lexer.curLine; lexer.curColumn = 1; continue; }
        [ \t\v\f]                   { ++lexer.curColumn; continue; }

        "//"                        { lexer.curColumn += 2;
                                      for (;;) {
                                          if (*lexer.cursor == 0)
                                              break;
                                          if (*lexer.cursor == '\r' || *lexer.cursor == '\n')
                                              break;
                                          ++lexer.cursor;
                                          ++lexer.curColumn;
                                      }
                                      emitToken(&lexer, T_SINGLE_LINE_COMMENT, "single line comment");
                                      continue;
                                    }
        "/*"                        { lexer.curColumn += 2;
                                      for (;;) {
                                          if (*lexer.cursor == 0) {
                                              CompilerToken* token = emitToken(&lexer, T_MULTI_LINE_COMMENT, "multi-line comment");
                                              // FIXME compilerError(compiler, &token->location, "unterminated comment.");
                                              break;
                                          }
                                          if (*lexer.cursor == '\r') {
                                              ++lexer.cursor;
                                              if (*lexer.cursor == '\n')
                                                  ++lexer.cursor;
                                              goto newline;
                                          }
                                          if (*lexer.cursor == '\n') {
                                              ++lexer.cursor;
                                            newline:
                                              lexer.curColumn = 1;
                                              ++lexer.curLine;
                                              continue;
                                          }
                                          if (lexer.cursor[0] == '*' && lexer.cursor[1] == '/') {
                                              lexer.cursor += 2;
                                              lexer.curColumn += 2;
                                              emitToken(&lexer, T_MULTI_LINE_COMMENT, "multi-line comment");
                                              break;
                                          }
                                          ++lexer.cursor;
                                          ++lexer.curColumn;
                                      }
                                      continue;
                                    }

        "abstract"                  { EMIT_KEYWORD(abstract); continue; }
        "bit"                       { EMIT_KEYWORD(bit); continue; }
        "bool"                      { EMIT_KEYWORD(bool); continue; }
        "break"                     { EMIT_KEYWORD(break); continue; }
        "byte"                      { EMIT_KEYWORD(byte); continue; }
        "case"                      { EMIT_KEYWORD(case); continue; }
        "catch"                     { EMIT_KEYWORD(catch); continue; }
        "class"                     { EMIT_KEYWORD(class); continue; }
        "const"                     { EMIT_KEYWORD(const); continue; }
        "continue"                  { EMIT_KEYWORD(continue); continue; }
        "default"                   { EMIT_KEYWORD(default); continue; }
        "delete"                    { EMIT_KEYWORD(delete); continue; }
        "do"                        { EMIT_KEYWORD(do); continue; }
        "else"                      { EMIT_KEYWORD(else); continue; }
        "enum"                      { EMIT_KEYWORD(enum); continue; }
        "false"                     { EMIT_KEYWORD(false); continue; }
        "final"                     { EMIT_KEYWORD(final); continue; }
        "finally"                   { EMIT_KEYWORD(finally); continue; }
        "flags"                     { EMIT_KEYWORD(flags); continue; }
        "for"                       { EMIT_KEYWORD(for); continue; }
        "friend"                    { EMIT_KEYWORD(friend); continue; }
        "goto"                      { EMIT_KEYWORD(goto); continue; }
        "if"                        { EMIT_KEYWORD(if); continue; }
        "int"                       { EMIT_KEYWORD(int); continue; }
        "int8"                      { EMIT_KEYWORD(int8); continue; }
        "int16"                     { EMIT_KEYWORD(int16); continue; }
        "int32"                     { EMIT_KEYWORD(int32); continue; }
        "interface"                 { EMIT_KEYWORD(interface); continue; }
        "new"                       { EMIT_KEYWORD(new); continue; }
        "null"                      { EMIT_KEYWORD(null); continue; }
        "object"                    { EMIT_KEYWORD(object); continue; }
        "override"                  { EMIT_KEYWORD(override); continue; }
        "private"                   { EMIT_KEYWORD(private); continue; }
        "protected"                 { EMIT_KEYWORD(protected); continue; }
        "public"                    { EMIT_KEYWORD(public); continue; }
        "sbyte"                     { EMIT_KEYWORD(sbyte); continue; }
        "sizeof"                    { EMIT_KEYWORD(sizeof); continue; }
        "static"                    { EMIT_KEYWORD(static); continue; }
        "struct"                    { EMIT_KEYWORD(struct); continue; }
        "switch"                    { EMIT_KEYWORD(switch); continue; }
        "throw"                     { EMIT_KEYWORD(throw); continue; }
        "true"                      { EMIT_KEYWORD(true); continue; }
        "try"                       { EMIT_KEYWORD(try); continue; }
        "uint8"                     { EMIT_KEYWORD(uint8); continue; }
        "uint16"                    { EMIT_KEYWORD(uint16); continue; }
        "uint32"                    { EMIT_KEYWORD(uint32); continue; }
        "union"                     { EMIT_KEYWORD(union); continue; }
        "var"                       { EMIT_KEYWORD(var); continue; }
        "void"                      { EMIT_KEYWORD(void); continue; }
        "while"                     { EMIT_KEYWORD(while); continue; }
        "word"                      { EMIT_KEYWORD(word); continue; }

        "."                         { EMIT(DOT, "."); continue; }
        ".."                        { EMIT(DOUBLE_DOT, ".."); continue; }
        "["                         { EMIT(LBRACKET, "["); continue; }
        "]"                         { EMIT(RBRACKET, "]"); continue; }
        "++"                        { EMIT(INCR, "++"); continue; }
        "--"                        { EMIT(DECR, "--"); continue; }
        ";"                         { EMIT(SEMICOLON, ";"); continue; }
        "&"                         { EMIT(AMPERSAND, "&"); continue; }
        "*"                         { EMIT(ASTERISK, "*"); continue; }
        "+"                         { EMIT(PLUS, "+"); continue; }
        "-"                         { EMIT(MINUS, "-"); continue; }
        "~"                         { EMIT(TILDE, "~"); continue; }
        "!"                         { EMIT(EXCLAMATION, "!"); continue; }
        "("                         { EMIT(LPAREN, "("); continue; }
        ")"                         { EMIT(RPAREN, ")"); continue; }
        "/"                         { EMIT(SLASH, "/"); continue; }
        "%"                         { EMIT(PERCENT, "%"); continue; }
        "<<"                        { EMIT(LSHIFT, "<<"); continue; }
        ">>"                        { EMIT(RSHIFT, ">>"); continue; }
        "<"                         { EMIT(LESS, "<"); continue; }
        ">"                         { EMIT(GREATER, ">"); continue; }
        "<="                        { EMIT(LESS_EQ, "<="); continue; }
        ">="                        { EMIT(GREATER_EQ, ">="); continue; }
        "=="                        { EMIT(EQUAL, "=="); continue; }
        "!="                        { EMIT(NOT_EQUAL, "!="); continue; }
        "^"                         { EMIT(CARET, "^"); continue; }
        "|"                         { EMIT(VBAR, "|"); continue; }
        "&&"                        { EMIT(DOUBLE_AMPERSAND, "&&"); continue; }
        "||"                        { EMIT(DOUBLE_VBAR, "||"); continue; }
        "?"                         { EMIT(QUESTION, "?"); continue; }
        ":"                         { EMIT(COLON, ":"); continue; }
        "="                         { EMIT(ASSIGN, "="); continue; }
        "+="                        { EMIT(ADD_ASSIGN, "+="); continue; }
        "-="                        { EMIT(SUB_ASSIGN, "-="); continue; }
        "*="                        { EMIT(MUL_ASSIGN, "*="); continue; }
        "/="                        { EMIT(DIV_ASSIGN, "/="); continue; }
        "%="                        { EMIT(MOD_ASSIGN, "%="); continue; }
        "|="                        { EMIT(OR_ASSIGN, "|="); continue; }
        "&="                        { EMIT(AND_ASSIGN, "&="); continue; }
        "^="                        { EMIT(XOR_ASSIGN, "^="); continue; }
        "<<="                       { EMIT(SHL_ASSIGN, "<<="); continue; }
        ">>="                       { EMIT(SHR_ASSIGN, ">>="); continue; }
        "{"                         { EMIT(LCURLY, "{"); continue; }
        "}"                         { EMIT(RCURLY, "}"); continue; }
        ","                         { EMIT(COMMA, ","); continue; }
        "@"                         { EMIT(ATSIGN, "@"); continue; }

        [a-zA-Z_][a-zA-Z0-9_]*      { size_t len = (size_t)(lexer.cursor - lexer.tokenStart);
                                      const char* text = compilerTempDupStrN(compiler, lexer.tokenStart, len);
                                      lexer.curColumn += (int)len;
                                      emitToken(&lexer, T_IDENTIFIER, "identifier")->text = text;
                                      continue;
                                    }

        "0b" [0-9a-zA-Z_]+          { uint_value_t value = 0, old;
                                      const char* p = lexer.tokenStart + 2;
                                      int len = (int)(lexer.cursor - lexer.tokenStart);
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
                                      } while (p != lexer.cursor);
                                      lexer.curColumn += len;
                                      if (error) {
                                          getLocation(&lexer, &loc);
                                          // FIXME compilerError(compiler, &loc, "syntax error in binary constant.");
                                      } else if (overflow) {
                                          getLocation(&lexer, &loc);
                                          // FIXME compilerWarning(compiler, &loc, "overflow while parsing octal constant.");
                                      }
                                      emitToken(&lexer, T_INTEGER_LITERAL, "binary constant")->integer = value;
                                      continue;
                                    }

        "0o" [0-9a-zA-Z_]+          { uint_value_t value = 0, old;
                                      const char* p = lexer.tokenStart + 2;
                                      int len = (int)(lexer.cursor - lexer.tokenStart);
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
                                      } while (p != lexer.cursor);
                                      lexer.curColumn += len;
                                      if (error) {
                                          getLocation(&lexer, &loc);
                                          // FIXME compilerError(compiler, &loc, "syntax error in octal constant.");
                                      } else if (overflow) {
                                          getLocation(&lexer, &loc);
                                          // FIXME compilerWarning(compiler, &loc, "overflow while parsing octal constant.");
                                      }
                                      emitToken(&lexer, T_INTEGER_LITERAL, "octal constant")->integer = value;
                                      continue;
                                    }

        "0x" [0-9a-zA-Z_]+          { uint_value_t value = 0, old;
                                      const char* p = lexer.tokenStart + 2;
                                      int len = (int)(lexer.cursor - lexer.tokenStart);
                                      bool error = false, overflow = false;
                                      do {
                                          old = value;
                                          value <<= 4;
                                          if (value < old)
                                              overflow = true;
                                          value |= digitToNumber(*p++, &error);
                                      } while (p != lexer.cursor);
                                      lexer.curColumn += len;
                                      if (error) {
                                          getLocation(&lexer, &loc);
                                          // FIXME compilerError(compiler, &loc, "syntax error in hexadecimal constant.");
                                      } else if (overflow) {
                                          getLocation(&lexer, &loc);
                                          // FIXME compilerWarning(compiler, &loc, "overflow while parsing octal constant.");
                                      }
                                      emitToken(&lexer, T_INTEGER_LITERAL, "hexadecimal constant")->integer = value;
                                      continue;
                                    }

        [0-9][0-9a-zA-Z_]*          { uint_value_t value = 0, old;
                                      const char* p = lexer.tokenStart;
                                      int len = (int)(lexer.cursor - lexer.tokenStart);
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
                                      } while (p != lexer.cursor);
                                      lexer.curColumn += len;
                                      if (error) {
                                          getLocation(&lexer, &loc);
                                          // FIXME compilerError(compiler, &loc, "syntax error in decimal constant.");
                                      } else if (overflow) {
                                          getLocation(&lexer, &loc);
                                          // FIXME compilerWarning(compiler, &loc, "overflow while parsing octal constant.");
                                      }
                                      emitToken(&lexer, T_INTEGER_LITERAL, "decimal constant")->integer = value;
                                      continue;
                                    }

        *                           { ++lexer.curColumn;
                                      getLocation(&lexer, &loc);
                                      ch = lexer.cursor[-1];
                                      /* FIXME
                                      if (ch >= 32 && ch < 127)
                                          compilerError(compiler, &loc, "unexpected symbol '%c'.", ch);
                                      else
                                          compilerError(compiler, &loc, "unexpected symbol.");
                                      */
                                    }

        */
    }
}
