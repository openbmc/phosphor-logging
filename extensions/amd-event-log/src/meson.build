ael_sources = files(
    'entry_points.cpp',
)

ael_lib = static_library(
    'amd-event-log',
    ael_sources,
    include_directories: include_directories('../../..', '../../../gen'),
    dependencies: [
        phosphor_logging_dep,
        conf_h_dep,
    ],
)

ael_dep = declare_dependency(
    link_with: ael_lib,
    dependencies: [phosphor_logging_dep],
)

log_manager_ext_deps += [ael_dep]

log_manager_ext_sources += files('entry_points.cpp')
