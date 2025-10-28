openpower_pels = {
    'additional_data': {},
    'ascii_string': {},
    'bcd_time': {},
    'data_interface': {},
    'device_callouts': {},
    'event_logger': {},
    'extended_user_data': {},
    'extended_user_header': {},
    'failing_mtms': {},
    'fru_identity': {},
    'generic_section': {},
    'host_notifier': {
        'sources': [
            '../../extensions/openpower-pels/host_notifier.cpp',
            '../../extensions/openpower-pels/repository.cpp',
        ],
    },
    'json_utils': {},
    'log_id': {},
    'mru': {},
    'mtms': {},
    'pce_identity': {},
    'pel_manager': {
        'sources': [
            '../../bmc_pos_mgr.cpp',
            '../../elog_entry.cpp',
            '../../elog_meta.cpp',
            '../../elog_serialize.cpp',
            '../../extensions.cpp',
            '../../log_manager.cpp',
            elog_lookup_gen,
            elog_process_gen,
            callouts_gen,
            generated_sources,
            log_manager_ext_sources,
        ],
        'deps': [cereal_dep],
    },
    'pel_rules': {},
    'pel': {},
    'pel_values': {},
    'private_header': {},
    'real_pel': {},
    'registry': {},
    'repository': {
        'sources': ['../../extensions/openpower-pels/repository.cpp'],
    },
    'section_header': {},
    'service_indicators': {},
    'severity': {},
    'src': {},
    'src_callout': {},
    'src_callouts': {},
    'stream': {},
    'user_data': {},
    'user_header': {},
    'temporary_file': {
        'sources': ['../../extensions/openpower-pels/temporary_file.cpp'],
    },
}

# Build a common shared library for all openpower tests of all the widely
# used sources under test.
openpower_test_lib = static_library(
    'openpower_test_lib',
    'pel_utils.cpp',
    'pel_paths.cpp',
    libpel_sources,
    peltool_sources,
    '../common.cpp',
    include_directories: include_directories('../../', '../../gen'),
    dependencies: [libpel_deps, peltool_deps, gtest_dep],
)

foreach t : openpower_pels.keys()
    test(
        'test_openpower_pels_' + t.underscorify(),
        executable(
            'test-openpower-pels-' + t.underscorify(),
            t + '_test.cpp',
            openpower_pels.get(t).get('sources', []),
            link_with: [openpower_test_lib],
            link_args: ['-lpython' + python_ver],
            dependencies: [
                gtest_dep,
                gmock_dep,
                phosphor_logging_dep,
                libpel_deps,
                log_manager_deps,
                peltool_deps,
                openpower_pels.get(t).get('deps', []),
            ],
            include_directories: include_directories('../../', '../../gen'),
        ),
    )
endforeach
