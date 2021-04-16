# Generate elog-errors.hpp.
elog_errors_gen = custom_target('elog-errors.hpp'.underscorify(),
    input: files(
        '../tools/elog-gen.py',
        '../tools/phosphor-logging/templates/elog-gen-template.mako.hpp',
    ),
    output: 'elog-errors.hpp',
    command: [
        python_prog, '@INPUT0@',
        '-t', '',
        '-m', '@INPUT1@',
        '-y', yamldir,
        '-u', meson.current_source_dir() / '../tools/',
        '-o', '@OUTPUT0@',
    ],
    install: true,
    install_dir: get_option('includedir') / 'phosphor-logging',
)

# Install headers.
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
