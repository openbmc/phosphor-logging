executable(
    'phosphor-rsyslog-conf',
    'main.cpp',
    'server-conf.cpp',
    '../elog_meta.cpp',
    '../util.cpp',
    elog_process_gen,
    callouts_gen,
    include_directories: include_directories('..'),
    dependencies: [
        conf_h_dep,
        phosphor_logging_dep,
        pdi_dep,
        sdbusplus_dep,
        sdeventplus_dep,
    ],
    install: true,
)
