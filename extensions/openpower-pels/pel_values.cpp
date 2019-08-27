#include "pel_values.hpp"

#include <algorithm>
#include <optional>

namespace openpower
{
namespace pels
{
namespace pel_values
{

// Note: The description fields will be filled in as part of the
//       PEL parser work.

/**
 * The possible values for the subsystem field  in the User Header.
 */
const PELValues subsystemValues = {{0x10, "processor", "TODO"},
                                   {0x11, "processor_fru", "TODO"},
                                   {0x12, "processor_chip", "TODO"},
                                   {0x13, "processor_unit", "TODO"},
                                   {0x14, "processor_bus", "TODO"},

                                   {0x20, "memory", "TODO"},
                                   {0x21, "memory_ctlr", "TODO"},
                                   {0x22, "memory_bus", "TODO"},
                                   {0x23, "memory_dimm", "TODO"},
                                   {0x24, "memory_fru", "TODO"},
                                   {0x25, "external_cache", "TODO"},

                                   {0x30, "io", "TODO"},
                                   {0x31, "io_hub", "TODO"},
                                   {0x32, "io_bridge", "TODO"},
                                   {0x33, "io_bus", "TODO"},
                                   {0x34, "io_processor", "TODO"},
                                   {0x35, "io_hub_other", "TODO"},
                                   {0x38, "phb", "TODO"},

                                   {0x40, "io_adapter", "TODO"},
                                   {0x41, "io_adapter_comm", "TODO"},
                                   {0x46, "io_device", "TODO"},
                                   {0x47, "io_device_dasd", "TODO"},

                                   {0x50, "cec_hardware", "TODO"},
                                   {0x51, "cec_sp_a", "TODO"},
                                   {0x52, "cec_sp_b", "TODO"},
                                   {0x53, "cec_node_controller", "TODO"},
                                   {0x55, "cec_vpd", "TODO"},
                                   {0x56, "cec_i2c", "TODO"},
                                   {0x57, "cec_chip_iface", "TODO"},
                                   {0x58, "cec_clocks", "TODO"},
                                   {0x59, "cec_op_panel", "TODO"},
                                   {0x5A, "cec_tod", "TODO"},
                                   {0x5B, "cec_storage_device", "TODO"},
                                   {0x5C, "cec_sp_hyp_iface", "TODO"},
                                   {0x5D, "cec_service_network", "TODO"},
                                   {0x5E, "cec_sp_hostboot_iface", "TODO"},

                                   {0x60, "power", "TODO"},
                                   {0x61, "power_supply", "TODO"},
                                   {0x62, "power_control_hw", "TODO"},
                                   {63, "power_fans", "TODO"},
                                   {0x64, "power_sequencer", "TODO"},

                                   {0x70, "others", "TODO"},
                                   {0x71, "other_hmc", "TODO"},
                                   {0x72, "other_test_tool", "TODO"},
                                   {0x73, "other_media", "TODO"},
                                   {0x74, "other_multiple_subsystems", "TODO"},
                                   {0x75, "other_na", "TODO"},
                                   {0x76, "other_info_src", "TODO"},

                                   {0x7A, "surv_sp_lost_hyp", "TODO"},
                                   {0x7C, "surv_sp_lost_hmc", "TODO"},

                                   {0x81, "bmc_firmware", "TODO"},
                                   {0x85, "spcn_firmware", "TODO"},

                                   {0xA1, "input_power_source", "TODO"},
                                   {0xA2, "ambient_temp", "TODO"},
                                   {0xA3, "user_error", "TODO"},
                                   {0xA4, "corrosion", "TODO"}};

/**
 * The possible values for the severity field in the User Header.
 */
const PELValues severityValues = {
    {0x00, "non_error", "TODO"},

    {0x10, "recovered", "TODO"},
    {0x20, "predictive", "TODO"},
    {0x21, "predictive_degraded_perf", "TODO"},
    {0x22, "predictive_reboot", "TODO"},
    {0x23, "predictive_reboot_degraded", "TODO"},
    {0x24, "predictive_redundancy_loss", "TODO"},

    {0x40, "unrecoverable", "TODO"},
    {0x41, "unrecoverable_degraded_perf", "TODO"},
    {0x44, "unrecoverable_redundancy_loss", "TODO"},
    {0x45, "unrecoverable_redundancy_loss_perf", "TODO"},
    {0x48, "unrecoverable_loss_of_function", "TODO"},

    {0x50, "critical", "TODO"},
    {0x51, "critical_system_term", "TODO"},
    {0x52, "critical_imminent_failure", "TODO"},
    {0x53, "critical_partition_term", "TODO"},
    {0x54, "critical_partition_imminent_failure", "TODO"},

    {0x60, "diagnostic_error", "TODO"},
    {0x61, "diagnostic_error_incorrect_results", "TODO"},

    {0x71, "symptom_recovered", "TODO"},
    {0x72, "symptom_predictive", "TODO"},
    {0x74, "symptom_unrecoverable", "TODO"},
    {0x75, "symptom_critical", "TODO"},
    {0x76, "symptom_diag_err", "TODO"}};

/**
 * The possible values for the Event Type field in the User Header.
 */
const PELValues eventTypeValues = {{0x00, "na", "TODO"},
                                   {0x01, "misc_information_only", "TODO"},
                                   {0x02, "tracing_event", "TODO"},
                                   {0x08, "dump_notification", "TODO"}};

/**
 * The possible values for the Event Scope field in the User Header.
 */
const PELValues eventScopeValues = {
    {0x01, "single_partition", "TODO"},
    {0x02, "multiple_partitions", "TODO"},
    {0x03, "entire_platform", "TODO"},
    {0x04, "possibly_multiple_platforms", "TODO"}};

/**
 * The possible values for the Action Flags field in the User Header.
 */
const PELValues actionFlagsValues = {
    {0x8000, "service_action", "TODO"}, {0x4000, "hidden", "TODO"},
    {0x2000, "report", "TODO"},         {0x1000, "dont_report", "TODO"},
    {0x0800, "call_home", "TODO"},      {0x0100, "termination", "TODO"}};

/**
 * The possible values for the Callout Priority field in the SRC.
 */
const PELValues calloutPriorityValues = {
    {'H', "high", "TODO"},           {'M', "medium", "TODO"},
    {'A', "medium_group_a", "TODO"}, {'B', "medium_group_b", "TODO"},
    {'C', "medium_group_c", "TODO"}, {'L', "low", "TODO"}};

PELValues::const_iterator findByValue(uint32_t value, const PELValues& fields)
{
    return std::find_if(fields.begin(), fields.end(),
                        [value](const auto& entry) {
                            return value == std::get<fieldValuePos>(entry);
                        });
}

PELValues::const_iterator findByName(const std::string& name,
                                     const PELValues& fields)

{
    return std::find_if(fields.begin(), fields.end(),
                        [&name](const auto& entry) {
                            return name == std::get<registryNamePos>(entry);
                        });
}

} // namespace pel_values

} // namespace pels
} // namespace openpower
