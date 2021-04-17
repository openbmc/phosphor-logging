project('phosphor-logging', 'cpp',
    meson_version: '>= 0.57.0',
    default_options: [
      'buildtype=debugoptimized',
      'cpp_std=c++20',
      'warning_level=3',
      'werror=true',
    ],
    version: '1.0.0',
)

python_prog = find_program('python3')

# Get sdbusplus dependency.
sdbusplus_dep = dependency('sdbusplus', required: false)
if sdbusplus_dep.found()
    sdbusplusplus_prog = find_program('sdbus++')
    sdbuspp_gen_meson_prog = find_program('sdbus++-gen-meson')
else
    sdbusplus_proj = subproject('sdbusplus', required: true)
    sdbusplus_dep = sdbusplus_proj.get_variable('sdbusplus_dep')
    sdbusplusplus_prog = sdbusplus_proj.get_variable('sdbusplusplus_prog')
    sdbuspp_gen_meson_prog = sdbusplus_proj.get_variable(
        'sdbuspp_gen_meson_prog'
    )
endif

# Get PDI and sdeventplus dependency.
pdi_dep = dependency(
    'phosphor-dbus-interfaces',
    fallback: ['phosphor-dbus-interfaces', 'phosphor_dbus_interfaces_dep'],
)
sdeventplus_dep = dependency(
    'sdeventplus',
    fallback: ['sdeventplus', 'sdeventplus_dep' ],
)

# Get Cereal dependency.
cereal_dep = dependency('cereal', required: false)
if not cereal_dep.found()
    subproject('cereal', required: false)
    cereal_dep = declare_dependency(
        include_directories: [ 'subprojects/cereal/include' ],
    )
endif

# Find the installed YAML directory, either from a configure option or
# by pulling it from the PDI dependency.
yamldir = get_option('yamldir')
if yamldir == ''
    yamldir = pdi_dep.get_variable(pkgconfig: 'yamldir', internal: 'yamldir')
endif

# Create config.h with constants that use to come from autoconf.
conf_data = configuration_data()
conf_data.set('error_cap', get_option('error_cap'))
conf_data.set('error_info_cap', get_option('error_info_cap'))
configure_file(
    input: 'config.h.meson',
    output: 'config.h',
    configuration: conf_data,
)

# Generate sdbus++ files.
generated_sources = []
generated_others = []
subdir('gen')
subdir('gen/xyz')

# Generate callouts-gen.hpp.
callouts_gen = custom_target('callouts-gen.hpp'.underscorify(),
    input: [
        'callouts/callouts.py',
        'callouts/callouts-gen.mako.hpp',
        get_option('callout_yaml'),
    ],
    output: 'callouts-gen.hpp',
    command: [ python_prog, '@INPUT0@', '-i', '@INPUT2@', '-o', '@OUTPUT0@' ],
)
# Generate elog-lookup.cpp
elog_lookup_gen = custom_target('elog-lookup.cpp'.underscorify(),
    input: files(
        'tools/elog-gen.py',
        'tools/phosphor-logging/templates/elog-lookup-template.mako.cpp',
    ),
    output: 'elog-lookup.cpp',
    command: [
        python_prog, '@INPUT0@',
        '-t', '',
        '-m', '@INPUT1@',
        '-y', yamldir,
        '-u', meson.current_source_dir() / '../tools/',
        '-o', '@OUTPUT0@',
    ],
)
# Generate elog-process-metadata.cpp
elog_process_gen = custom_target('elog-process-metadata.cpp'.underscorify(),
    input: files(
        'tools/elog-gen.py',
        'tools/phosphor-logging/templates/elog-process-metadata.mako.cpp',
    ),
    output: 'elog-process-metadata.cpp',
    command: [
        python_prog, '@INPUT0@',
        '-t', '',
        '-m', '@INPUT1@',
        '-y', yamldir,
        '-u', meson.current_source_dir() / '../tools/',
        '-o', '@OUTPUT0@',
    ],
)

subdir('phosphor-logging')
subdir('phosphor-rsyslog-config')

# Build libphosphor-logging.
libphosphor_logging = library(
    'phosphor_logging',
    files(
        'elog.cpp',
        'elog_meta.cpp',
        'sdjournal.cpp',
    ),
    callouts_gen,
    elog_errors_gen,
    dependencies: [ sdbusplus_dep, pdi_dep, sdeventplus_dep ],
    version: meson.project_version(),
    install: true,
)

import('pkgconfig').generate(
    libphosphor_logging,
    name: meson.project_name(),
    version: meson.project_version(),
    requires: [
        'libsystemd',
        'phosphor-dbus-interfaces',
        'sdbusplus',
    ],
    description: 'Phosphor logging utilities',
)

# Create dependency for use as subproject.
phosphor_logging_dep = declare_dependency(
    include_directories: include_directories('.'),
    link_with: libphosphor_logging,
    sources: [ callouts_gen, elog_errors_gen ],
    dependencies: [ sdbusplus_dep, pdi_dep, sdeventplus_dep],
)

# Generate daemon.
log_manager_sources = [
    generated_sources,
    elog_lookup_gen,
    elog_process_gen,
    files(
        'elog_entry.cpp',
        'elog_meta.cpp',
        'elog_serialize.cpp',
        'extensions.cpp',
        'log_manager.cpp',
        'sdjournal.cpp',
        'util.cpp',
    )
]
executable('phosphor-log-manager',
    log_manager_sources,
    'log_manager_main.cpp',
    include_directories: include_directories('gen'),
    link_with: libphosphor_logging,
    dependencies: [
        cereal_dep,
        pdi_dep,
        sdbusplus_dep,
        sdeventplus_dep
    ],
    install: true,
)
# Generate test executables which run in obmc env (qemu, real hardware).
executable('logging-test',
    'logging_test.cpp',
    link_with: libphosphor_logging,
    dependencies: [
        elog_errors_dep,
        pdi_dep,
        sdbusplus_dep,
    ],
    install: true,
)
executable('callout-test',
    'callouts/callout_test.cpp',
    link_with: libphosphor_logging,
    dependencies: [
        elog_errors_dep,
        pdi_dep,
        sdbusplus_dep,
        sdeventplus_dep,
    ],
    install: true,
)

if not get_option('tests').disabled()
    subdir('test')
endif
