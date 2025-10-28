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
            ],
        )
        gmock_dep = gtest_proj.dependency('gmock')
    else
        assert(
            not get_option('tests').enabled(),
            'Googletest is required if tests are enabled',
        )
    endif
endif

if get_option('openpower-pel-extension').allowed()
    subdir('openpower-pels')
endif

tests = [
    'bmc_pos_mgr_test',
    'extensions_test',
    'log_manager_dbus_tests',
    'remote_logging_test_address',
    'remote_logging_test_config',
    'remote_logging_test_port',
    'sdtest',
    'serialization_test_path',
    'serialization_test_properties',
]
foreach t : tests
    test(
        'test_' + t.underscorify(),
        executable(
            'test-' + t.underscorify(),
            t + '.cpp',
            'common.cpp',
            '../phosphor-rsyslog-config/server-conf.cpp',
            dependencies: [
                conf_h_dep,
                gmock_dep,
                gtest_dep,
                log_manager_deps,
                pdi_dep,
                phosphor_logging_dep,
            ],
            include_directories: include_directories('..', '../gen'),
            link_with: log_manager_lib,
        ),
    )
endforeach

tests_non_parallel = [
    'elog_quiesce_test',
    'elog_update_ts_test',
    'elog_errorwrap_test',
]

foreach t : tests_non_parallel
    test(
        'test_' + t.underscorify(),
        executable(
            'test-' + t.underscorify(),
            t + '.cpp',
            'common.cpp',
            '../phosphor-rsyslog-config/server-conf.cpp',
            dependencies: [
                conf_h_dep,
                gmock_dep,
                gtest_dep,
                log_manager_deps,
                phosphor_logging_dep,
            ],
            include_directories: include_directories('..', '../gen'),
            link_with: log_manager_lib,
        ),
        is_parallel: false,
    )
endforeach
