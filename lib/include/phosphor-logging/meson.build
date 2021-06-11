phosphor_logging_gen += custom_target(
    'elog-errors.hpp'.underscorify(),
    input: [elog_gen, template_elog_gen],
    output: 'elog-errors.hpp',
    command: [
        python_prog, '@INPUT0@',
        '-t', '',
        '-m', '@INPUT1@',
        '-y', yamldir,
        '-u', tool_dir + '/',
        '-o', '@OUTPUT0@',
    ],
    install: true,
    install_dir: get_option('includedir') / 'phosphor-logging',
)

install_headers(
    'elog.hpp',
    'log.hpp',
    'sdjournal.hpp',
    subdir: 'phosphor-logging',
)

install_headers(
    'test/sdjournal_mock.hpp',
    subdir: 'phosphor-logging/test',
)
