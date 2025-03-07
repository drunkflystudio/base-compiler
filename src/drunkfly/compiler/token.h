#ifndef DRUNKFLY_COMPILER_TOKEN_H
#define DRUNKFLY_COMPILER_TOKEN_H

#include <drunkfly/compiler.h>

#define T_EOF 0
#include "y.tab.h"

struct CompilerToken {
    const char* text;
    const char* name;
    const uint32_t* str;
    size_t strLength;
    uint32_t symbol;
    CompilerLocation location;
    uint_value_t integer;
    short id;
    char overflow; /* bool */
};

#endif
