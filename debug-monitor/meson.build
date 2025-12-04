# debug-monitor/meson.build
systemd_dep = dependency('libsystemd')

# Complile debug-monitor execution
debug_monitor_exe = executable(
    'debug-monitor',
    'debug-monitor.cpp',
    dependencies: [systemd_dep],
    install: true,
    install_dir: get_option('bindir'),
    install_tag: 'debug-monitor',
)

# Install systemd service
systemd_system_unit_dir = dependency('systemd').get_variable(
    'systemdsystemunitdir',
)

configure_file(
    input: 'debug-monitor.service.in',
    output: 'debug-monitor.service',
    configuration: {'bindir': get_option('prefix') / get_option('bindir')},
    install: true,
    install_dir: systemd_system_unit_dir,
    install_tag: 'debug-monitor',
)

# Install systemd preset to enable service by default
systemd_system_preset_dir = dependency('systemd').get_variable(
    'systemdsystempresetdir',
)
install_data(
    '../systemd-preset/90-debug-monitor.preset',
    install_dir: systemd_system_preset_dir,
)

# variable for upper meson.build
debug_monitor = declare_dependency()
