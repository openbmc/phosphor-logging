
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

systemd_system_unit_dir = systemd.get_variable(
        pkgconfig: 'systemdsystemunitdir',
        pkgconfig_define: ['prefix', get_option('prefix')])

configure_file(input: 'services/xyz.openbmc_project.Logging.Audit.service',
                output: 'xyz.openbmc_project.Logging.Audit.service',
                copy: true,
                install_dir: systemd_system_unit_dir)