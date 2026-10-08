// Snippets that misuse the lg2 API. Each CASE_* macro selects one snippet
// that must fail to compile; lg2_compile_fail.py compiles this file once per
// case and checks the compiler diagnostic. Without any CASE_* macro, the file
// contains valid code that must compile, which guards against a broken
// compiler command line making every negative case pass.

#include <phosphor-logging/lg2.hpp>

#include <string>

void lg2CompileFail()
{
#if defined(CASE_TRAILING_HEADER)
    lg2::info("m", "KEY");
#elif defined(CASE_FLAG_BEFORE_HEADER)
    lg2::info("m", lg2::hex, "KEY", 5);
#elif defined(CASE_MISALIGNED_PAIRS)
    lg2::info("m", "A", 1, 2, "B", 3);
#elif defined(CASE_LOWERCASE_HEADER)
    lg2::info("m", "key", 1);
#elif defined(CASE_RESERVED_HEADER)
    lg2::info("m", "MESSAGE", 1);
#elif defined(CASE_PROHIBITED_FLAG)
    lg2::info("m", "KEY", lg2::dec, std::string("str"));
#else
    lg2::info("m", "KEY", std::string("value"), "HEX", lg2::hex, 5);
#endif
}
