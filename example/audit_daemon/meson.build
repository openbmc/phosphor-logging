c_args = [
    '-O2',
    '-Wall',
    '-Wno-comment',
    '-Wno-unused-parameter',
    '-Wno-unused-variable',
    '-Wno-unused-but-set-variable',
    '-pipe',
    '-fpic',
    '-feliminate-unused-debug-types',
]

cpp_args = []

# Include Directories
auditd_inc = include_directories(['include', '../../lib/include', '../../gen'])

# Sources
auditd_src = (
    [
            generated_sources,
    files(
        'src/audit_config.cpp',
        'src/audit_main.cpp',
        'src/audit_manager.cpp',
        'src/auditd.cpp',
            ),
            ]
)

# Deps
auditd_dep = declare_dependency(
    link_args: [
        '-lsdeventplus',
        '-lsdbusplus',
        '-lsystemd',
        '-lm',
        '-laudit',
        '-lfmt',
    ],
    include_directories: include_directories('.'),
)

boost_version = '>=1.88.0'
boost_modules = []
boost_args = []

boost_deps = dependency('boost', version: boost_version, modules: boost_modules)

executable(
    'phosphor-auditd',
    auditd_src,
    c_args: c_args,
    cpp_args: cpp_args,
    include_directories: auditd_inc,
    dependencies: [pdi_dep, boost_deps, auditd_dep],
    link_with: phosphor_logging_lib,
    install: true,
)

subdir('include')
