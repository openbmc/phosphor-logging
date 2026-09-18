#include "processor.hpp"

#include <phosphor-logging/lg2.hpp>

namespace phosphor::logging::cper
{
PHOSPHOR_LOG2_USING;

void Processor::method_call(process_t, sdbusplus::object_path source,
                            ContentType type, sdbusplus::message::unix_fd data)
{
    info("CPER Process request: source={SOURCE} type={TYPE} fd={FD}", "SOURCE",
         source.str, "TYPE", type, "FD", static_cast<int>(data));
}

} // namespace phosphor::logging::cper
