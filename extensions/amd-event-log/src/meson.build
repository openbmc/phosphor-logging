ael_sources = files(
    'entry_points.cpp',
)

ael_inc = include_directories('.')

ael_lib = static_library(
    'amd-event-log',
    ael_sources,
    include_directories: ael_inc,
    dependencies: [
        phosphor_logging_dep,
    ],
)
