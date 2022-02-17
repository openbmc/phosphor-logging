
audit_sources = [
    generated_sources,
    'audit_main.cpp',
    'audit_manager.cpp',
    'file.cpp',
    'utils.cpp',
]

executable(
    'phosphor-audit',
    audit_sources,
    include_directories: ['.', '../', '../gen'],
    dependencies: [
        pdi_dep,
        phosphor_logging_dep,
        sdbusplus_dep,
        sdeventplus_dep,
    ],
    install: true,
    install_dir: get_option('bindir')
)
