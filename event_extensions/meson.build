event_extensions_sources = files(
    'builtins.cpp',
    'registry.cpp',
    'manager.cpp',
)

# Interfaces used by the CPER extensions. Keep this list in sync with the
# __has_include() checks in builtins.cpp.
cper_interfaces = [
    'xyz/openbmc_project/Logging/CPER/Raw',
    'xyz/openbmc_project/Logging/CPER/Types',
    'xyz/openbmc_project/Logging/Extension/CPER/Processed',
]

has_cper = true
foreach intf : cper_interfaces
    if pdi_dep.type_name() == 'internal'
        has_cper = has_cper and fs.is_file(
            yamldir / (intf + '.interface.yaml'),
        )
    else
        has_cper = has_cper and cpp.has_header(
            intf / 'common.hpp',
            dependencies: pdi_dep,
        )
    endif
endforeach

if has_cper
    subdir('cper')
endif
subdir('utils')
