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
if get_option('amd-event-log').allowed()
    subdir('amd-event-log')
endif

subdir('event_extensions')

tests = [
    'bmc_pos_mgr_test',
    'extensions_test',
    'lg2_test',
    'remote_logging_test_address',
    'remote_logging_test_config',
    'remote_logging_test_port',
    'sdtest',
    'serialization_test_json',
    'serialization_test_path',
    'serialization_test_properties',
]

# log_manager_dbus_tests uses the Logging.Cleared event from
# <xyz/openbmc_project/Logging/event.hpp>, which older
# phosphor-dbus-interfaces releases do not provide.
if pdi_dep.type_name() == 'internal'
    has_logging_event_hpp = fs.is_file(
        yamldir / 'xyz/openbmc_project/Logging.events.yaml',
    )
else
    has_logging_event_hpp = cpp.has_header(
        'xyz/openbmc_project/Logging/event.hpp',
        dependencies: pdi_dep,
    )
endif

if has_logging_event_hpp
    tests += ['log_manager_dbus_tests']
endif

if get_option('redundant-bmc')
    tests += ['log_manager_rbmc_sync_test']
endif

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

# lg2_logger.cpp is compiled into this test (instead of linking the shared
# library) and listed after the test source so that the static initializer in
# the test runs before the dynamic initializers of lg2_logger.cpp.
test(
    'test_lg2_static_init',
    executable(
        'test-lg2_static_init',
        'lg2_static_init_test.cpp',
        '../lib/lg2_logger.cpp',
        dependencies: [libsystemd_dep, sdbusplus_dep],
        include_directories: phosphor_logging_includes,
    ),
    env: ['LG2_FORCE_STDERR=1'],
)

if build_log_create
    log_create_extensions_test = executable(
        'test-log-create-extensions',
        'log_create_extensions_test.cpp',
        '../log_create_extensions.cpp',
        dependencies: [
            conf_h_dep,
            gtest_dep,
            nlohmann_json_dep,
            pdi_dep,
            sdbusplus_dep,
        ],
        include_directories: include_directories('..'),
    )
    test('test_log_create_extensions', log_create_extensions_test)
endif

# Verify that misuse of the lg2 API is rejected at compile time with the
# expected diagnostic. Reuses the compiler command of lg2_test.cpp from
# compile_commands.json.
test(
    'test_lg2_compile_fail',
    python_prog,
    args: [
        files('lg2_compile_fail.py'),
        meson.project_build_root(),
        files('lg2_compile_fail.cpp'),
    ],
    timeout: 300,
)

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
