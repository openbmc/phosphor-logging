# Include Directories
audit_api_usage_inc = include_directories(
  [
    '../../lib/include',
    '../../gen/'
  ]
)

# Sources
audit_api_usage_src = (
  [
    'audit_api_usage.cpp'
  ]
)

# Deps
audit_api_usage_dep = declare_dependency(
    link_args : [
      # sdeventplus
      '-lsdeventplus',

      # sdbusplus
      '-lsdbusplus',

      # systemd
      '-lsystemd',

      # math
      '-lm',
    ],

    include_directories : include_directories('.')
)

executable(
  'audit_api_usage',
  audit_api_usage_src,
  include_directories: audit_api_usage_inc,
  dependencies: [audit_api_usage_dep, phosphor_logging_dep],
  link_with: phosphor_logging_lib,
  export_dynamic: true,
  install : true
)
