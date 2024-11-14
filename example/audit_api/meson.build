# Include Directories

audit_api_usage_inc = include_directories(
  [
    '../../lib/include',
    '../../gen'
  ]
)

audit_api_usage_src = (
  [
    'audit_api_usage.cpp'
  ]
)

audit_api_usage_dep = declare_dependency(
    link_args : [
      # sdeventplus
      '-lsdeventplus',

      # sdbusplus
      '-lsdbusplus',

      # stdplus
      #'-lstdplus',
      
      # phosphor
      '-lphosphor_logging',

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
  dependencies: [audit_api_usage_dep],
  export_dynamic: true,
  #debug: true,
  install : true
)
