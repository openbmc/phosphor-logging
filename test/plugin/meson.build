test(
    'test_plugin_registry',
    executable(
        'test-plugin-registry',
        'plugin_registry_test.cpp',
        link_with: [log_manager_lib],
        dependencies: [
            gtest_dep,
            gmock_dep,
            log_manager_deps,
        ],
        include_directories: include_directories(
            '../../',
            '../../gen',
        ),
    ),
)
