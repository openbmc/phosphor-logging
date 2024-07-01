# Create config.h with constants that use to come from autoconf.
conf_data = configuration_data()
conf_data.set('error_cap', get_option('error_cap'))
conf_data.set('error_info_cap', get_option('error_info_cap'))
conf_data.set('rsyslog_server_conf', get_option('rsyslog_server_conf'))

if(get_option('transport-implementation')) == 'mctp-demux'
   add_project_arguments('-DPLDM_TRANSPORT_WITH_MCTP_DEMUX', language : 'cpp')
else
   add_project_arguments('-DPLDM_TRANSPORT_WITH_AF_MCTP', language: 'cpp')
endif

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
    )
)
