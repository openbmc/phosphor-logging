#pragma once

#include <map>
#include <string>
#include <vector>

namespace phosphor::logging::extensions::ael
{

using AdditionalDataMap = std::map<std::string, std::string>;

/**
 * Parse phosphor-logging AdditionalData entries of the form:
 *
 * KEY=value
 */
AdditionalDataMap parseAdditionalData(
    const std::vector<std::string>& additionalData);

} // namespace phosphor::logging::extensions::ael
