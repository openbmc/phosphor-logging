sdbusplus_dep = dependency('sdbusplus')

executable(
    'amd-log-manager',
    [
        'main.cpp',
        'amd_log_manager.cpp',
    ],
    dependencies: [
        sdbusplus_dep,
        phosphor_logging_dep,
        sdeventplus_dep,
    ],
    install: true,
)

# Install systemd service
systemd = dependency('systemd')

systemd_unit_dir = systemd.get_variable('systemdsystemunitdir')

install_data(
    'service/amd-log-manager.service',
    install_dir: systemd_unit_dir,
)
