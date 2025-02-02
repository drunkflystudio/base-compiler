#ifndef DRUNKFLY_COMPILER_TOKEN_H
#define DRUNKFLY_COMPILER_TOKEN_H

#include <drunkfly/compiler.h>

#define T_EOF 0
#include "y.tab.h"

typedef uint32_t uint_value_t;
#define UINT_VALUE_FMT "%" PRINTF_INT32_MODIFIER "u"

struct CompilerToken {
    const char* text;
    const char* name;
    CompilerLocation location;
    uint_value_t integer;
    short id;
    char overflow; /* bool */
};

#endif
