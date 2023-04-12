systemd_system_unit_dir = dependency('systemd').get_variable(
    'systemdsystemunitdir')

install_data(
    ['xyz.openbmc_project.Logging.service',
     'xyz.openbmc_project.Syslog.Config.service'],
    install_dir: systemd_system_unit_dir,
)

install_data(
    'busconfig/phosphor-logging.conf',
    'busconfig/phosphor-rsyslog-config.conf',
    install_dir: get_option('datadir') / 'dbus-1' / 'system.d',
)

dbus_system_bus_services_dir = dependency('dbus-1').get_variable(
    'system_bus_services_dir',
    pkgconfig_define: ['prefix', get_option('prefix')])
install_data(
    'dbus/xyz.openbmc_project.Logging.service',
    install_dir: dbus_system_bus_services_dir)
