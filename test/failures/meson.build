# Verify that misuse of the lg2 API is rejected at compile time. Each
# snippet below must fail to compile; configuration fails otherwise.
#
# Meson only accepts external dependencies in compiler checks. Hence pass
# the lg2 include directory explicitly and skip these checks if sdbusplus,
# which lg2.hpp includes, is built as a subproject.
if sdbusplus_dep.type_name() == 'internal'
    message('sdbusplus is a subproject; skipping lg2 compile-failure checks')
    subdir_done()
endif

lg2_check_args = {
    'include_directories': phosphor_logging_includes,
    'dependencies': sdbusplus_dep,
}

lg2_prologue = '''#include <phosphor-logging/lg2.hpp>

void lg2CompileFail()
{
'''

# Valid code must compile, so that a broken check cannot make every negative
# case below pass.
assert(
    cpp.compiles(
        lg2_prologue + '''
    lg2::info("m", "KEY", "value", "HEX", lg2::hex, 5);
}''',
        name: 'lg2 accepts a valid invocation',
        kwargs: lg2_check_args,
    ),
    'lg2: valid invocation did not compile',
)

lg2_negative_cases = {
    'TRAILING_HEADER': 'lg2::info("m", "KEY");',
    'TRAILING_HEADER_FLAG': 'lg2::info("m", "KEY", lg2::hex);',
    'FLAG_BEFORE_HEADER': 'lg2::info("m", lg2::hex, "KEY", 5);',
    'MISALIGNED_PAIRS': 'lg2::info("m", "A", 1, 2, "B", 3);',
    'LOWERCASE_HEADER': 'lg2::info("m", "key", 1);',
    'RESERVED_HEADER': 'lg2::info("m", "MESSAGE", 1);',
    'PROHIBITED_FLAG': 'lg2::info("m", "KEY", lg2::dec, "str");',
}

foreach name, snippet : lg2_negative_cases
    assert(
        not cpp.compiles(
            lg2_prologue + '    ' + snippet + '\n}',
            name: 'lg2 rejects ' + name,
            kwargs: lg2_check_args,
        ),
        'lg2: misuse unexpectedly compiled: ' + name,
    )
endforeach
