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
