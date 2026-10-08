// Verify that lg2 can be used from the static initializer of another
// translation unit, before the dynamic initializers of lg2_logger.cpp have
// run.
//
// This test is built from this file and ../lib/lg2_logger.cpp, in that order,
// so that the static initializer below runs before those of lg2_logger.cpp.

#include <phosphor-logging/lg2.hpp>

namespace
{

struct EarlyLogger
{
    EarlyLogger()
    {
        lg2::info("Logged from a static initializer: {VALUE}", "VALUE", 42);
    }
};

const EarlyLogger earlyLogger;

} // namespace

int main()
{
    return 0;
}
