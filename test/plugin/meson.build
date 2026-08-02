test(
    'test_plugin_cper_descriptor',
    executable(
        'test-plugin-cper-descriptor',
        'cper_descriptor_test.cpp',
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

test(
    'test_plugin_manager',
    executable(
        'test-plugin-manager',
        'plugin_manager_test.cpp',
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

test(
    'test_plugin_cper',
    executable(
        'test-plugin-cper',
        'cper_plugin_test.cpp',
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
