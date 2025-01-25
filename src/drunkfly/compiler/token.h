#ifndef DRUNKFLY_COMPILER_TOKEN_H
#define DRUNKFLY_COMPILER_TOKEN_H

#include <drunkfly/compiler.h>

#define T_EOF 0

typedef uint32_t uint_value_t;
typedef unsigned long uint_value_fmt;
#define UINT_VALUE_FMT "%" PRINTF_INT32_MODIFIER "u"

struct Token {
    Token* next;
    const char* text;
    const char* name;
    uint_value_t integer;
    int id;
};

#endif
