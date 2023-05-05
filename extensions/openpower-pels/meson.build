libpldm_dep = dependency(
    'libpldm',
    default_options: ['oem-ibm=enabled'],
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
    CLI11_dep = dependency('CLI11')
endif

extra_sources = []
extra_dependencies = []
extra_args = []

build_phal = get_option('phal').enabled()

if build_phal
    extra_sources += [
        'sbe_ffdc_handler.cpp',
        'fapi_data_process.cpp',
        'phal_service_actions.cpp',
    ]
    extra_dependencies += [
        dependency('libdt-api'),
        cpp.find_library('pdbg'),
        cpp.find_library('ekb'),
        cpp.find_library('phal'),
    ]
    extra_args += [
        '-DPEL_ENABLE_PHAL',
    ]
    log_manager_ext_args += [
        '-DPEL_ENABLE_PHAL',
    ]
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
    'journal.cpp',
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
    'temporary_file.cpp',
    extra_sources,
)

libpel_deps = [
    conf_h_dep,
    libpldm_dep,
    nlohmann_json,
    sdbusplus_dep,
    sdeventplus_dep,
    pdi_dep,
    phosphor_logging_dep,
    extra_dependencies,
]

libpel_lib = static_library(
    'pel',
    libpel_sources,
    'paths.cpp', # paths is separate because it is overridden during test.
    include_directories: include_directories('../..', '../../gen'),
    cpp_args: extra_args,
    dependencies: [
        libpel_deps,
    ]
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
        phosphor_logging_dep,
    ]
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
    'pel_entry.cpp',
    'pldm_interface.cpp',
    'repository.cpp',
    'src.cpp',
    'user_data.cpp',
)

install_data(
    'registry/message_registry.json',
    'registry/O_component_ids.json',
    'registry/B_component_ids.json',
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
    conf_h_dep,
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
