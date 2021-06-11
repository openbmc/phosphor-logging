template_elog_gen = files('elog-gen-template.mako.hpp')
template_elog_lookup = files('elog-lookup-template.mako.cpp')
template_elog_process_metadata = files('elog-process-metadata.mako.cpp')

install_data(
    template_elog_gen,
    template_elog_lookup,
    template_elog_process_metadata,
    install_dir : get_option('datadir') / 'phosphor-logging' / 'elog' /
        'tools' / 'phosphor-logging' / 'templates',
)
