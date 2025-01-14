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

cpp_args = [
    '-DBOOST_ASIO_DISABLE_CONCEPTS',
    '-DBOOST_ALL_NO_LIB',
    '-DBOOST_ALLOW_DEPRECATED_HEADERS',
    '-DBOOST_ASIO_DISABLE_THREADS',
    '-DBOOST_ASIO_NO_DEPRECATED',
    '-DBOOST_ASIO_SEPARATE_COMPILATION',
    '-DBOOST_BEAST_SEPARATE_COMPILATION',
    '-DBOOST_EXCEPTION_DISABLE',
    '-DBOOST_URL_NO_SOURCE_LOCATION',
    '-DJSON_NOEXCEPTION',
    '-DOPENSSL_NO_FILENAMES',
    '-DSDBUSPLUS_DISABLE_BOOST_COROUTINES',
]

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

boost_version = '>=1.82.0'
boost_modules = []

boost_args = [
    '-DBOOST_ASIO_DISABLE_THREADS',
    '-DBOOST_ALL_NO_LIB',
    '-DBOOST_SYSTEM_NO_DEPRECATED',
    '-DBOOST_ASIO_NO_DEPRECATED',
    '-DBOOST_NO_RTTI',
    '-DBOOST_NO_TYPEID',
]

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
