# Generated file; do not modify.
subdir('Manager')
generated_others += custom_target(
    'xyz/openbmc_project/Logging/Internal/Manager__markdown'.underscorify(),
    input: [ '../../../../../xyz/openbmc_project/Logging/Internal/Manager.interface.yaml',  ],
    output: [ 'Manager.md' ],
    command: [
        sdbuspp_gen_meson_prog, '--command', 'markdown',
        '--output', meson.current_build_dir(),
        '--tool', sdbusplusplus_prog,
        '--directory', meson.current_source_dir() / '../../../../..',
        'xyz/openbmc_project/Logging/Internal/Manager',
    ],
)

