gtest_dep = dependency('gtest', main: true, disabler: true, required: false)
gmock_dep = dependency('gmock', disabler: true, required: false)
if not gtest_dep.found() or not gmock_dep.found()
    gtest_proj = import('cmake').subproject('googletest', required: false)
    if gtest_proj.found()
        gtest_dep = declare_dependency(
            dependencies: [
                dependency('threads'),
                gtest_proj.dependency('gtest'),
                gtest_proj.dependency('gtest_main'),
            ]
        )
        gmock_dep = gtest_proj.dependency('gmock')
  else
        assert(
            not get_option('tests').enabled(),
            'Googletest is required if tests are enabled'
        )
  endif
endif

if not get_option('openpower-pel-extension').disabled()
    subdir('openpower-pels')
endif

tests = [
    'elog_quiesce_test',
    'elog_update_ts_test',
    'extensions_test',
    'remote_logging_test_address',
    'remote_logging_test_config',
    'remote_logging_test_port',
    'sdtest',
    'serialization_test_path',
    'serialization_test_properties',
    'elog_errorwrap_test',
]
foreach t : tests
    test(
        'test_' + t.underscorify(),
        executable(
            'test-' + t.underscorify(),
            t + '.cpp',
            'common.cpp',
            log_manager_sources,
            '../phosphor-rsyslog-config/server-conf.cpp',
            dependencies: [
                gmock_dep,
                gtest_dep,
                log_manager_deps,
                phosphor_logging_dep,
            ],
            include_directories: include_directories('..', '../gen'),
        )
    )
endforeach
