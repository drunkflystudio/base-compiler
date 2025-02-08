
test_bootstrap([[
class Application {
    public static (void) main {
    }
}
]], [[
#include <drunkfly/common.h>

struct Application; typedef struct Application Application;

struct Application
{
};

static void
Application_main()
{
}
]])
