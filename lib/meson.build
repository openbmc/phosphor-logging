phosphor_logging_includes = include_directories('include')

phosphor_logging_gen = []

subdir('include/phosphor-logging')

threads_dep = dependency('threads')

phosphor_logging_deps = [
    pdi_dep,
    sdbusplus_dep,
    systemd_dep,
    threads_dep
]

phosphor_logging_lib = library(
    'phosphor_logging',
    'elog.cpp',
    'lg2_logger.cpp',
    'sdjournal.cpp',
    phosphor_logging_gen,
    implicit_include_directories: false,
    include_directories: phosphor_logging_includes,
    dependencies: [
        phosphor_logging_deps,
        conf_h_dep,
    ],
    version: meson.project_version(),
    install: true,
)

phosphor_logging_reqs = []
foreach dep : phosphor_logging_deps
    if dep.type_name() == 'pkgconfig'
        phosphor_logging_reqs += dep
    endif
endforeach

import('pkgconfig').generate(
    phosphor_logging_lib,
    name: 'phosphor-logging',
    version: meson.project_version(),
    requires: phosphor_logging_reqs,
    description: 'Phosphor logging utilities',
)

phosphor_logging_dep = declare_dependency(
    include_directories: phosphor_logging_includes,
    link_with: phosphor_logging_lib,
    sources: phosphor_logging_gen,
    dependencies: phosphor_logging_deps,
)

