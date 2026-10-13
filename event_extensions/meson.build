event_extensions_sources = files(
    'builtins.cpp',
    'registry.cpp',
    'manager.cpp',
)

has_cper = false
if pdi_dep.type_name() == 'internal'
    has_cper = fs.is_file(
        yamldir / 'xyz/openbmc_project/Logging/CPER/Types.interface.yaml',
    )
else
    has_cper = cpp.has_header(
        'xyz/openbmc_project/Logging/CPER/Types/common.hpp',
        dependencies: pdi_dep,
    )
endif

if has_cper
    subdir('cper')
endif
subdir('utils')
