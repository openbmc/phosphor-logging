libpldm_dep = dependency(
    'libpldm',
    fallback: ['pldm', 'libpldm_dep'],
    default_options: ['libpldm-only=enabled', 'oem-ibm=enabled'],
)

if cpp.has_header('nlohmann/json.hpp')
    nlohmann_json = declare_dependency()
else
    subproject('nlohmann-json')
    nlohmann_json = declare_dependency(
        include_directories: include_directories(
            '../../subprojects/nlohmann-json/single_include',
            '../../subprojects/nlohmann-json/single_include/nlohmann',
        )
    )
endif

python_inst = import('python').find_installation('python3')
python_ver = python_inst.language_version()
python_dep = python_inst.dependency()

if cpp.has_header('CLI/CLI.hpp')
    CLI11_dep = declare_dependency()
else
    CLI11_dep = dependency(
        'CLI11',
        fallback: [ 'CLI11', 'CLI11_dep' ],
    )
endif

libpel_sources = files(
    'ascii_string.cpp',
    'bcd_time.cpp',
    'callout.cpp',
    'callouts.cpp',
    'data_interface.cpp',
    'device_callouts.cpp',
    'extended_user_header.cpp',
    'failing_mtms.cpp',
    'fru_identity.cpp',
    'generic.cpp',
    'json_utils.cpp',
    'log_id.cpp',
    'mru.cpp',
    'mtms.cpp',
    'pce_identity.cpp',
    'pel.cpp',
    'pel_rules.cpp',
    'pel_values.cpp',
    'private_header.cpp',
    'registry.cpp',
    'section_factory.cpp',
    'service_indicators.cpp',
    'severity.cpp',
    'user_header.cpp',
)

libpel_deps = [
    libpldm_dep,
    nlohmann_json,
    sdbusplus_dep,
    sdeventplus_dep,
    pdi_dep,
]

libpel_lib = static_library(
    'pel',
    libpel_sources,
    'paths.cpp', # paths is separate because it is overridden during test.
    include_directories: include_directories('../..', '../../gen'),
    dependencies: libpel_deps,
)

libpel_dep = declare_dependency(
    include_directories: include_directories('.'),
    link_with: libpel_lib,
    dependencies: [
        libpldm_dep,
        nlohmann_json,
        sdbusplus_dep,
        sdeventplus_dep,
        pdi_dep,
    ],

)

log_manager_ext_deps += [
    libpel_dep,
    libpldm_dep,
    nlohmann_json,
]

log_manager_ext_sources += files(
    'entry_points.cpp',
    'extended_user_data.cpp',
    'host_notifier.cpp',
    'manager.cpp',
    'pldm_interface.cpp',
    'repository.cpp',
    'src.cpp',
    'user_data.cpp',
)

install_data(
    'registry/message_registry.json',
    install_dir: get_option('datadir') / 'phosphor-logging/pels',
)

peltool_sources = files(
    'extended_user_data.cpp',
    'src.cpp',
    'user_data.cpp',
    'user_data_json.cpp',
)

peltool_deps = [
    CLI11_dep,
    python_dep,
]

executable(
    'peltool',
    'tools/peltool.cpp',
    peltool_sources,
    cpp_args: [ '-DPELTOOL' ],
    link_args: [ '-lpython' + python_ver ],
    include_directories: include_directories('../..'),
    dependencies: [
        peltool_deps,
        libpel_dep,
    ],
    install: true,
)
