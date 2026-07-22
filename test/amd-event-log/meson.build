test(
    'ael_info_test',
    executable(
        'ael_info_test',
        [
            'ael_info_test.cpp',
            '../../extensions/amd-event-log/src/ael_info.cpp',
            '../../extensions/amd-event-log/src/amd_afid_utils.cpp',
            lut_gen,
        ],
        include_directories: [
            include_directories('../../extensions/amd-event-log/include'),
        ],
        dependencies: [
            gtest_dep,
            phosphor_logging_dep,
            conf_h_dep,
        ],
    ),
)
