tool_dir = meson.current_source_dir()

elog_gen = files('elog-gen.py')
install_data(
    elog_gen,
    install_dir: get_option('datadir') / 'phosphor-logging' / 'elog' / 'tools',
)

subdir('phosphor-logging/templates')
