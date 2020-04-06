#pragma once

#include "xyz/openbmc_project/Logging/ErrorBlocksTransition/server.hpp"

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>

namespace phosphor
{
namespace logging
{

using BlockIface = sdbusplus::server::object::object<
    sdbusplus::xyz::openbmc_project::Logging::server::ErrorBlocksTransition>;

/** @class Block
 *  @brief OpenBMC logging Block implementation.
 *  @details A concrete implementation for the
 *  xyz.openbmc_project.Logging.ErrorBlocksTransition DBus API
 */
class Block : public BlockIface
{
  public:
    Block() = delete;
    Block(const Block&) = delete;
    Block& operator=(const Block&) = delete;
    Block(Block&&) = delete;
    Block& operator=(Block&&) = delete;
    virtual ~Block() = default;

    /** @brief Constructor to put object onto bus at a dbus path.
     *  @param[in] bus - Bus to attach to.
     *  @param[in] path - Path to attach at.
     *  @param[in] entryId - Distinct ID of the error.
     */
    Block(sdbusplus::bus::bus& bus, const std::string& path, uint32_t entryId) :
        BlockIface(bus, path.c_str()), entryId(entryId){};

    uint32_t entryId;

  private:
};

} // namespace logging
} // namespace phosphor
