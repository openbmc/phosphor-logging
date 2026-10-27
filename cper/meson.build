libcper_dep = declare_dependency(
    dependencies: [
        dependency(
            'libcper',
            fallback: ['libcper', 'libcper_parse_dep'],
            include_type: 'system',
        ),
        dependency('json-c', include_type: 'system'),
    ],
)

executable(
    'phosphor-cper-processor',
    'decoder.cpp',
    'oem_registry.cpp',
    'processor.cpp',
    'processor_main.cpp',
    dependencies: [
        CLI11_dep,
        libcper_dep,
        nlohmann_json_dep,
        pdi_dep,
        phosphor_logging_dep,
        sdbusplus_dep,
    ],
    export_dynamic: true,
    install: true,
    install_dir: get_option('libexecdir') / 'phosphor-logging',
)
