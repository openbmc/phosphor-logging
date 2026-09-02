test(
    'test_event_extensions',
    executable(
        'test-event-extensions',
        'event_extensions_test.cpp',
        link_with: [log_manager_lib],
        dependencies: [
            gtest_dep,
            log_manager_deps,
        ],
        include_directories: include_directories(
            '../../',
            '../../gen',
        ),
    ),
)

test(
    'test_cper_processed',
    executable(
        'test-cper-processed',
        'cper_processed_test.cpp',
        link_with: [log_manager_lib],
        dependencies: [
            gtest_dep,
            log_manager_deps,
        ],
        include_directories: include_directories(
            '../../',
            '../../gen',
        ),
    ),
)

test(
    'test_artifact',
    executable(
        'test-artifact',
        'artifact_test.cpp',
        link_with: [
            log_manager_lib,
        ],
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
    'test_cper_raw',
    executable(
        'test-cper-raw',
        'cper_raw_test.cpp',
        link_with: [log_manager_lib],
        dependencies: [
            gtest_dep,
            log_manager_deps,
        ],
        include_directories: include_directories(
            '../../',
            '../../gen',
        ),
    ),
)
