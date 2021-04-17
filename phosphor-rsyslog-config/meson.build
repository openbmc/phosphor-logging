executable('phosphor-rsyslog-conf',
    'main.cpp',
    'server-conf.cpp',
    '../sdjournal.cpp',
    elog_process_gen,
    include_directories: include_directories('..'),
    dependencies: [
        elog_errors_dep,
        pdi_dep,
        sdbusplus_dep,
        sdeventplus_dep,
    ],
    install: true,
)
