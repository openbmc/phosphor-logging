systemd_system_unit_dir = dependency('systemd').get_pkgconfig_variable(
    'systemdsystemunitdir',
    define_variable: ['prefix', get_option('prefix')])

install_data(
    ['xyz.openbmc_project.Logging.service',
     'xyz.openbmc_project.Syslog.Config.service'],
    install_dir: systemd_system_unit_dir
)

install_data(
    ['busconfig/phosphor-logging.conf',
     'busconfig/phosphor-rsyslog-config.conf'],
    install_dir: get_option('datadir') / 'dbus-1' / 'system.d'
)
