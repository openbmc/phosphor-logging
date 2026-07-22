python = import('python').find_installation(required: true)

lut_json_file = meson.current_source_dir() / '../data/reverse_lut.json'

lut_script = files('../codegen/generate_lut.py')[0]

lut_gen = custom_target(
    'amd_afid_db_gen',
    output: 'amd_afid_db_gen.hpp',
    command: [
        python,
        lut_script,
        '--input',
        lut_json_file,
        '--output',
        '@OUTPUT@',
    ],
    depend_files: [
        lut_json_file,
        lut_script,
    ],
    build_by_default: true,
)

ael_sources = files(
    'ael_info.cpp',
    'amd_afid_utils.cpp',
    'entry_points.cpp',
)

ael_inc = include_directories('../include')

log_manager_ext_sources += ael_sources
log_manager_ext_sources += [lut_gen]

log_manager_ext_deps += [
    phosphor_logging_dep,
    conf_h_dep,
]

log_manager_ext_inc += [
    include_directories('../include'),
]
