project(
    'phosphor-logging',
    'cpp',
    meson_version: '>=1.1.1',
    default_options: [
        'buildtype=debugoptimized',
        'cpp_std=c++23',
        'warning_level=3',
        'werror=true',
        'libonly=' + (meson.is_subproject() ? 'true' : 'false'),
        'tests=' + (meson.is_subproject() ? 'disabled' : 'auto'),
    ],
    version: '1.0.0',
)
cpp = meson.get_compiler('cpp')

python_prog = find_program('python3', native: true)
libsystemd_dep = dependency('libsystemd')

sdbusplus_dep = dependency('sdbusplus')
sdbusplusplus_prog = find_program('sdbus++', native: true)
sdbuspp_gen_meson_prog = find_program('sdbus++-gen-meson', native: true)
sdbusplusplus_depfiles = files()
if sdbusplus_dep.type_name() == 'internal'
    sdbusplusplus_depfiles = subproject('sdbusplus').get_variable(
        'sdbusplusplus_depfiles',
    )
endif

pdi_dep = dependency('phosphor-dbus-interfaces')

# Find the installed YAML directory, either from a configure option or
# by pulling it from the PDI dependency.
yamldir = get_option('yamldir')
if yamldir == ''
    yamldir = pdi_dep.get_variable('yamldir')
endif

subdir('config')
subdir('tools')
subdir('lib')

if get_option('libonly')
    subdir_done()
endif

sdeventplus_dep = dependency('sdeventplus')

# Get Cereal dependency.
cereal_dep = dependency('cereal', required: false)
has_cereal = cpp.has_header_symbol(
    'cereal/cereal.hpp',
    'cereal::specialize',
    dependencies: cereal_dep,
    required: false,
)
if not has_cereal
    cereal_opts = import('cmake').subproject_options()
    cereal_opts.add_cmake_defines(
        {'BUILD_TESTS': 'OFF', 'SKIP_PERFORMANCE_COMPARISON': 'ON'},
    )
    cereal_proj = import('cmake').subproject(
        'cereal',
        options: cereal_opts,
        required: false,
    )
    assert(cereal_proj.found(), 'cereal is required')
    cereal_dep = cereal_proj.dependency('cereal')
endif

# Get CLI11 dependency
if cpp.has_header('CLI/CLI.hpp')
    CLI11_dep = declare_dependency()
else
    CLI11_dep = dependency('CLI11')
endif

nlohmann_json_dep = dependency('nlohmann_json', include_type: 'system')

# Generate sdbus++ files.
should_generate_cpp = true
should_generate_markdown = false
should_generate_registry = false
yaml_selected_subdirs = ['xyz']
subdir('gen')

# Generate callouts-gen.hpp.
callouts_gen = custom_target(
    'callouts-gen.hpp'.underscorify(),
    input: [
        'callouts/callouts.py',
        'callouts/callouts-gen.hpp.mako',
        get_option('callout_yaml'),
    ],
    output: 'callouts-gen.hpp',
    command: [python_prog, '@INPUT0@', '-i', '@INPUT2@', '-o', '@OUTPUT0@'],
)
# Generate elog-lookup.cpp
elog_lookup_gen = custom_target(
    'elog-lookup.cpp'.underscorify(),
    input: files(
        'tools/elog-gen.py',
        'tools/phosphor-logging/templates/elog-lookup-template.cpp.mako',
    ),
    output: 'elog-lookup.cpp',
    command: [
        python_prog,
        '@INPUT0@',
        '-t',
        '',
        '-m',
        '@INPUT1@',
        '-y',
        yamldir,
        '-u',
        meson.current_source_dir() / 'tools/',
        '-o',
        '@OUTPUT0@',
    ],
)
# Generate elog-process-metadata.cpp
elog_process_gen = custom_target(
    'elog-process-metadata.cpp'.underscorify(),
    input: files(
        'tools/elog-gen.py',
        'tools/phosphor-logging/templates/elog-process-metadata.cpp.mako',
    ),
    output: 'elog-process-metadata.cpp',
    command: [
        python_prog,
        '@INPUT0@',
        '-t',
        '',
        '-m',
        '@INPUT1@',
        '-y',
        yamldir,
        '-u',
        meson.current_source_dir() / 'tools/',
        '-o',
        '@OUTPUT0@',
    ],
)

log_manager_ext_sources = []
log_manager_ext_deps = []
log_manager_ext_args = []

subdir('extensions')
subdir('phosphor-rsyslog-config')

# Generate daemon.
log_manager_sources = [
    generated_sources,
    callouts_gen,
    elog_lookup_gen,
    elog_process_gen,
    files(
        'elog_entry.cpp',
        'elog_meta.cpp',
        'elog_serialize.cpp',
        'extensions.cpp',
        'log_manager.cpp',
        'paths.cpp',
        'util.cpp',
    ),
]
log_manager_deps = [
    cereal_dep,
    conf_h_dep,
    pdi_dep,
    phosphor_logging_dep,
    sdbusplus_dep,
    sdeventplus_dep,
]
log_manager_lib = static_library(
    'log_manager_lib',
    log_manager_sources,
    log_manager_ext_sources,
    include_directories: include_directories('gen'),
    dependencies: [log_manager_deps, log_manager_ext_deps],
    cpp_args: log_manager_ext_args,
)
# log_manager_deps needs the generated headers as a dependency
log_manager_deps = declare_dependency(
    sources: generated_headers,
)
executable(
    'phosphor-log-manager',
    'log_manager_main.cpp',
    include_directories: include_directories('gen'),
    cpp_args: log_manager_ext_args,
    dependencies: [conf_h_dep, log_manager_deps, phosphor_logging_dep],
    install: true,
    link_with: log_manager_lib
)

executable(
    'log-create',
    'log_create_main.cpp',
    dependencies: [
        CLI11_dep,
        nlohmann_json_dep,
        pdi_dep,
        phosphor_logging_dep,
        sdbusplus_dep,
    ],
    install: true,
)

executable(
    'log-resolve',
    'log_resolve_main.cpp',
    dependencies: [CLI11_dep, pdi_dep, phosphor_logging_dep],
    install: true,
)

# Generate test executables which run in obmc env (qemu, real hardware).
executable(
    'logging-test',
    'logging_test.cpp',
    dependencies: [pdi_dep, phosphor_logging_dep, sdbusplus_dep],
    install: true,
)
executable(
    'callout-test',
    'callouts/callout_test.cpp',
    dependencies: [
        conf_h_dep,
        pdi_dep,
        phosphor_logging_dep,
        sdbusplus_dep,
        sdeventplus_dep,
    ],
    install: true,
)

subdir('dist')

if get_option('tests').allowed()
    subdir('test')
endif
