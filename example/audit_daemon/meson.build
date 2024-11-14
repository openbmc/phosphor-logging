project(
  'phosphor-audit',
  'cpp',
  version : '0.1',

  default_options : [
    'buildtype=debug',
    'werror=false',
    'warning_level=2',
    'cpp_std=c++23'
  ])

# Get compiler and default build type

cxx = meson.get_compiler('cpp')

  add_global_arguments([
      #'-fanalyzer',
      '-O2',
      '-Wall',
      #'-Wcpp',
      '-Wno-comment',
      #'-Werror',
      #'-Wnon-virtual-dtor',
      #'-Wunused-parameter',
      '-Wno-unused-parameter',
      #'-Wunused-variable',
      '-Wno-unused-variable',
      '-Wno-unused-but-set-variable',
      '-g',
      '-pipe',
      '-fpic',
      '-feliminate-unused-debug-types',
    ],
    language: 'cpp'
  )

# Boost dependency configuration

add_project_arguments(
cxx.get_supported_arguments([
  '-DBOOST_ASIO_DISABLE_CONCEPTS',
  '-DBOOST_ALL_NO_LIB',
  '-DBOOST_ALLOW_DEPRECATED_HEADERS',
  '-DBOOST_ASIO_DISABLE_THREADS',
  '-DBOOST_ASIO_NO_DEPRECATED',
  '-DBOOST_ASIO_SEPARATE_COMPILATION',
  '-DBOOST_BEAST_SEPARATE_COMPILATION',
  '-DBOOST_EXCEPTION_DISABLE',
 # '-DBOOST_NO_EXCEPTIONS',
  '-DBOOST_URL_NO_SOURCE_LOCATION',
  '-DJSON_NOEXCEPTION',
  '-DOPENSSL_NO_FILENAMES',
  '-DSDBUSPLUS_DISABLE_BOOST_COROUTINES',
]),
language : 'cpp')

# Include Directories

auditd_inc = include_directories(
  [
    'include',
    '../../gen'
  ]
)

auditd_src = (
  [
    'src/audit_manager.cpp',
    'src/audit_config.cpp',
    'src/audit_main.cpp',
    'src/linux_audit.cpp',
    '../../gen/xyz/openbmc_project/Logging/Audit/Manager/server.cpp'
  ]
)

auditd_dep = declare_dependency(
    link_args : [
      # sdeventplus
      '-lsdeventplus',

      # sdbusplus
      '-lsdbusplus',

      # stdplus
      #'-lstdplus',

      # phosphor
      #'-lphosphor_dbus',
      '-lphosphor_logging',

      # systemd
      '-lsystemd',

      # math
      '-lm',

      # libaudit (linux-audit user space)
      '-laudit'
    ],

    include_directories : include_directories('.')
)

cxx = meson.get_compiler('cpp')
add_project_arguments(
    cxx.get_supported_arguments([
        '-DBOOST_ASIO_DISABLE_THREADS',
        '-DBOOST_ALL_NO_LIB',
        '-DBOOST_SYSTEM_NO_DEPRECATED',
        '-DBOOST_ASIO_NO_DEPRECATED',
        '-DBOOST_NO_RTTI',
        '-DBOOST_NO_TYPEID',
        '-Wno-unused-parameter',
    ]),
    language: 'cpp'
)

boost_version = '>=1.82.0'
boost_modules = []
boost_deps = dependency('boost',
    version: boost_version,
    modules: boost_modules)


install_headers(
    'audit_config.hpp',
    'audit_manager.hpp',
    'linux_audit.hpp',
    subdir: 'phosphor-logging/auditd'
)


executable(
  'phosphor-auditd',
  auditd_src,
  include_directories: auditd_inc,
  dependencies: [boost_deps, auditd_dep],
  #debug: true,
  install : true
)
