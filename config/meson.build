# Create config.h with constants that use to come from autoconf.
conf_data = configuration_data()
conf_data.set('error_cap', get_option('error_cap'))
conf_data.set('error_info_cap', get_option('error_info_cap'))
conf_data.set('rsyslog_server_conf', get_option('rsyslog_server_conf'))

lg2_commit_strategy = get_option('lg2_commit_strategy')
conf_data.set(
    'lg2_commit_dbus',
    lg2_commit_strategy == 'dbus' or lg2_commit_strategy == 'both' ? 'true' : 'false',
)
conf_data.set(
    'lg2_commit_journal',
    lg2_commit_strategy == 'journal' or lg2_commit_strategy == 'both' ? 'true' : 'false',
)
conf_data.set(
    'lg2_commit_deprecated_rf_format',
    get_option('lg2_commit_create_deprecated_redfish_format').allowed() ? 'true' : 'false',
)

cxx = meson.get_compiler('cpp')
if cxx.has_header('poll.h')
    add_project_arguments('-DPLDM_HAS_POLL=1', language: 'cpp')
endif

conf_h_dep = declare_dependency(
    include_directories: include_directories('.'),
    sources: configure_file(
        input: 'config.h.meson',
        output: 'config.h',
        configuration: conf_data,
    ),
)
