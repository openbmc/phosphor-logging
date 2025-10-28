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

use_bmc_pos_in_id = get_option('use-bmc-pos-in-id')
conf_data.set10('use_bmc_pos_in_id', use_bmc_pos_in_id)

bmc_pos_path = get_option('bmc-position-object-path')
conf_data.set('bmc_pos_obj_path', bmc_pos_path)

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
