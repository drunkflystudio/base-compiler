
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
    if (tmp5 != tmp4)
        tmp5 <<= 0x4;
    if (tmp1)
        tmp3 -= tmp4;
    else
        {
            uint8_t x;
            x = 0xc;
            break;
            continue;
        }
}
]])
