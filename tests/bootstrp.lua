
test_bootstrap([[
class Application
{
    public static
        (void)
        main
    {
        var (bool) tmp1;
        var (bool) tmp2 = false;
        var (word) tmp3 = (4);
        var (word) tmp4 = tmp3[tmp3];
        const (word) tmp5 = tmp4 / 123456;
        tmp5 += (9 + 3 * 8) - tmp4++ + --tmp3 % 24 << 6;
        tmp1 = (tmp3 > tmp4 ? tmp3 < tmp4 : tmp3 == tmp4) || tmp3 && tmp4;
        ;
      label:
        goto label;

        if (tmp5 != tmp4)
            tmp5 <<= 4;
        if (tmp1)
            tmp3 -= tmp4;
        else {
            var (byte) x = 12;
            break;
            continue;
        }

        if (var (word) x = 4; x == 4)
            var (word) y = 5;

        while (true)
            ;

        while (var (bool) x = false; x) {
            var (bool) y = true;
            if (y)
                break;
        }

        do {
        } while (false);

        do {
            var (word) w = 4;
        } while (w == 4);

        do
            var (word) q = 12;
        while (q != 12);

        for (;;)
            break;

        for (var (word) x;;)
            break;

        for (var (word) x = 7; x; x++)
            break;

        for (tmp4 = 11; ; x++)
            break;

        for (tmp4 = 11; tmp4 != 12; )
            break;

        for (; tmp4 != 12; )
            break;

        for (;; ++tmp4)
            break;

        switch (tmp4) {
        }

        switch (tmp4) {
            case 4:
                break;
        }

        switch (tmp4) {
            case 5:
            case 6:
                break;
        }

        switch (var (word) x = 12; x) {
            case 5:
                continue;
            default:
                var (word) y = 8;
            case 6:
            case 7:
                var (word) w = 9;
                break;
        }

        return;
        return 4 + 8;
    }
}
]], [[
#include <drunkfly/common.h>

#line 1 "file"
STRUCT(Application);

#line 1 "file"
struct Application
#line 2 "file"
{
#line 2 "file"
    int (*dispatch)(lua_State* L);
#line 100 "file"
};

#line 4 "file"
void
#line 5 "file"
Application_main(lua_State* L) { UNUSED(L);
#line 6 "file"
{
#   line 7 "file"
    bool tmp1;
#   line 8 "file"
    bool tmp2;
#   line 9 "file"
    uint16_t tmp3;
#   line 10 "file"
    uint16_t tmp4;
#   line 11 "file"
    uint16_t const tmp5;
#   line 8 "file"
    tmp2 = false;
#   line 9 "file"
    tmp3 = (0x4);
#   line 10 "file"
    tmp4 = tmp3[tmp3];
#   line 11 "file"
    tmp5 = tmp4 / 0x1e240;
#   line 12 "file"
    tmp5 += (0x9 + 0x3 * 0x8) - tmp4++ + --tmp3 % 0x18 << 0x6;
#   line 13 "file"
    tmp1 = (tmp3 > tmp4 ? tmp3 < tmp4 : tmp3 == tmp4) || tmp3 && tmp4;
#   line 14 "file"
    ;
#   line 15 "file"
    label:;
#   line 16 "file"
    goto label;
#   line 18 "file"
    {
#       line 18 "file"
        if (tmp5 != tmp4) {
#           line 19 "file"
            tmp5 <<= 0x4;
        }
    }
#   line 20 "file"
    {
#       line 20 "file"
        if (tmp1) {
#           line 21 "file"
            tmp3 -= tmp4;
#       line 22 "file"
        } else {
#           line 22 "file"
            {
#               line 23 "file"
                uint8_t x;
#               line 23 "file"
                x = 0xc;
#               line 24 "file"
                break;
#               line 25 "file"
                continue;
#           line 26 "file"
            }
        }
    }
#   line 28 "file"
    {
#       line 28 "file"
        uint16_t x;
#       line 28 "file"
        x = 0x4;
#       line 28 "file"
        if (x == 0x4) {
#           line 29 "file"
            uint16_t y;
#           line 29 "file"
            y = 0x5;
        }
    }
#   line 31 "file"
    {
#       line 31 "file"
        while (true) {
#           line 32 "file"
            ;
        }
    }
#   line 34 "file"
    {
#       line 34 "file"
        bool x;
#       line 34 "file"
        x = false;
#       line 34 "file"
        while (x) {
#           line 34 "file"
            {
#               line 35 "file"
                bool y;
#               line 35 "file"
                y = true;
#               line 36 "file"
                {
#                   line 36 "file"
                    if (y) {
#                       line 37 "file"
                        break;
                    }
                }
#           line 38 "file"
            }
        }
    }
#   line 40 "file"
    {
#       line 40 "file"
        do {
#       line 41 "file"
        } while (false);
    }
#   line 43 "file"
    {
#       line 44 "file"
        uint16_t w;
#       line 43 "file"
        do {
#           line 44 "file"
            w = 0x4;
#       line 45 "file"
        } while (w == 0x4);
    }
#   line 47 "file"
    {
#       line 48 "file"
        uint16_t q;
#       line 47 "file"
        do {
#           line 48 "file"
            q = 0xc;
#       line 49 "file"
        } while (q != 0xc);
    }
#   line 51 "file"
    {
#       line 51 "file"
        for (; ; ) {
#           line 52 "file"
            break;
        }
    }
#   line 54 "file"
    {
#       line 54 "file"
        uint16_t x;
#       line 54 "file"
        for (; ; ) {
#           line 55 "file"
            break;
        }
    }
#   line 57 "file"
    {
#       line 57 "file"
        uint16_t x;
#       line 57 "file"
        x = 0x7;
#       line 57 "file"
        for (; x; x++) {
#           line 58 "file"
            break;
        }
    }
#   line 60 "file"
    {
#       line 60 "file"
        for (tmp4 = 0xb; ; x++) {
#           line 61 "file"
            break;
        }
    }
#   line 63 "file"
    {
#       line 63 "file"
        for (tmp4 = 0xb; tmp4 != 0xc; ) {
#           line 64 "file"
            break;
        }
    }
#   line 66 "file"
    {
#       line 66 "file"
        for (; tmp4 != 0xc; ) {
#           line 67 "file"
            break;
        }
    }
#   line 69 "file"
    {
#       line 69 "file"
        for (; ; ++tmp4) {
#           line 70 "file"
            break;
        }
    }
#   line 72 "file"
    {
#       line 72 "file"
        switch (tmp4) {
        }
    }
#   line 75 "file"
    {
#       line 75 "file"
        switch (tmp4) {
#           line 76 "file"
            case 0x4:
            {
#               line 77 "file"
                break;
            }
        }
    }
#   line 80 "file"
    {
#       line 80 "file"
        switch (tmp4) {
#           line 81 "file"
            case 0x5:
#           line 82 "file"
            case 0x6:
            {
#               line 83 "file"
                break;
            }
        }
    }
#   line 86 "file"
    {
#       line 86 "file"
        uint16_t x;
#       line 86 "file"
        x = 0xc;
#       line 86 "file"
        switch (x) {
#           line 87 "file"
            case 0x5:
            {
#               line 88 "file"
                continue;
#           line 89 "file"
            }
#           line 89 "file"
            default:
            {
#               line 90 "file"
                uint16_t y;
#               line 90 "file"
                y = 0x8;
#           line 91 "file"
            }
#           line 91 "file"
            case 0x6:
#           line 92 "file"
            case 0x7:
            {
#               line 93 "file"
                uint16_t w;
#               line 93 "file"
                w = 0x9;
#               line 94 "file"
                break;
            }
        }
    }
#   line 97 "file"
    return ;
#   line 98 "file"
    return 0x4 + 0x8;
#line 99 "file"
}
}
]])

test_bootstrap([[
class Application
{
    public static (void) main
    {
        var (string) str;
    }
}
]], [[
#include <drunkfly/common.h>

#line 1 "file"
STRUCT(Application);

#line 1 "file"
struct Application
#line 2 "file"
{
#line 2 "file"
    int (*dispatch)(lua_State* L);
#line 7 "file"
};

#line 3 "file"
void
#line 3 "file"
Application_main(lua_State* L) { UNUSED(L);
#line 4 "file"
{
#   line 5 "file"
    char* str;
#line 6 "file"
}
}
]])
