phosphor_logging_includes = include_directories('include')

event_filter_json = get_option('event-filter')
if event_filter_json == ''
    event_filter_json = default_eventfilter_json
endif

lg2_eventfilter_cpp_gen = custom_target(
    'lg2_eventfilter.cpp',
    input: [eventfilter_gen, template_eventfilter_gen, event_filter_json],
    output: 'lg2_eventfilter.cpp',
    command: [python_prog, '@INPUT0@', '@INPUT2@'],
    capture: true,
)

phosphor_logging_gen = [lg2_eventfilter_cpp_gen]

subdir('include/phosphor-logging')

threads_dep = dependency('threads')

phosphor_logging_deps = [libsystemd_dep, pdi_dep, sdbusplus_dep, threads_dep]

phosphor_logging_lib = library(
    'phosphor_logging',
    'elog.cpp',
    'lg2_commit.cpp',
    'lg2_logger.cpp',
    'sdjournal.cpp',
    phosphor_logging_gen,
    implicit_include_directories: false,
    include_directories: phosphor_logging_includes,
    dependencies: [phosphor_logging_deps, conf_h_dep],
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

if meson.is_subproject()
    phosphor_logging_dep = phosphor_logging_dep.as_system()
endif
