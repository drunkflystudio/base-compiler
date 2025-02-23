
test_bootstrap([[
class Application {
    public static (void) main {
        var tmp1 : bool;
        var tmp2 : bool = false;
        var tmp3 : word = (4);
        var tmp4 : word = tmp3[tmp3];
        const tmp5 : word = tmp4 / 123456;
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
            var x : byte = 12;
            break;
            continue;
        }

        if (var x : word = 4; x == 4)
            var y : word = 5;

        while (true)
            ;

        while (var x : bool = false; x) {
            var y : bool = true;
            if (y)
                break;
        }

        do {
        } while (false);

        do {
            var w : word = 4;
        } while (w == 4);

        do
            var q : word = 12;
        while (q != 12);

        for (;;)
            break;

        for (var x : word;;)
            break;

        for (var x : word = 7; x; x++)
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

        switch (var x : word = 12; x) {
            case 5:
                continue;
            default:
                var y : word = 8;
            case 6:
            case 7:
                var w : word = 9;
                break;
        }
    }
}
]], [[
#include <drunkfly/common.h>

STRUCT(Application);

struct Application
{
};

static void
Application_main()
{
    bool tmp1;
    bool tmp2;
    uint16_t tmp3;
    uint16_t tmp4;
    uint16_t const tmp5;
    tmp2 = false;
    tmp3 = (0x4);
    tmp4 = tmp3[tmp3];
    tmp5 = tmp4 / 0x1e240;
    tmp5 += (0x9 + 0x3 * 0x8) - tmp4++ + --tmp3 % 0x18 << 0x6;
    tmp1 = (tmp3 > tmp4 ? tmp3 < tmp4 : tmp3 == tmp4) || tmp3 && tmp4;
    ;
    label:;
    goto label;
    {
        if (tmp5 != tmp4) {
            tmp5 <<= 0x4;
        }
    }
    {
        if (tmp1) {
            tmp3 -= tmp4;
        } else {
            {
                uint8_t x;
                x = 0xc;
                break;
                continue;
            }
        }
    }
    {
        uint16_t x;
        x = 0x4;
        if (x == 0x4) {
            uint16_t y;
            y = 0x5;
        }
    }
    {
        while (true) {
            ;
        }
    }
    {
        bool x;
        x = false;
        while (x) {
            {
                bool y;
                y = true;
                {
                    if (y) {
                        break;
                    }
                }
            }
        }
    }
    {
        do {
        } while (false);
    }
    {
        uint16_t w;
        do {
            w = 0x4;
        } while (w == 0x4);
    }
    {
        uint16_t q;
        do {
            q = 0xc;
        } while (q != 0xc);
    }
    {
        for (; ; ) {
            break;
        }
    }
    {
        uint16_t x;
        for (; ; ) {
            break;
        }
    }
    {
        uint16_t x;
        x = 0x7;
        for (; x; x++) {
            break;
        }
    }
    {
        for (tmp4 = 0xb; ; x++) {
            break;
        }
    }
    {
        for (tmp4 = 0xb; tmp4 != 0xc; ) {
            break;
        }
    }
    {
        for (; tmp4 != 0xc; ) {
            break;
        }
    }
    {
        for (; ; ++tmp4) {
            break;
        }
    }
    {
        switch (tmp4) {
        }
    }
    {
        switch (tmp4) {
            case 0x4:
            {
                break;
            }
        }
    }
    {
        switch (tmp4) {
            case 0x5:
            case 0x6:
            {
                break;
            }
        }
    }
    {
        uint16_t x;
        x = 0xc;
        switch (x) {
            case 0x5:
            {
                continue;
            }
            default:
            {
                uint16_t y;
                y = 0x8;
            }
            case 0x6:
            case 0x7:
            {
                uint16_t w;
                w = 0x9;
                break;
            }
        }
    }
}
]])
