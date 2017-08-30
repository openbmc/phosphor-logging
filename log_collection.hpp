#pragma once

#include <sdbusplus/bus.hpp>
#include <sdbusplus/server/object.hpp>
#include "xyz/openbmc_project/Collection/server.hpp"
namespace phosphor
{
namespace logging
{

using CollectionIfaces = sdbusplus::server::object::object<
    sdbusplus::xyz::openbmc_project::server::Collection>;

class Manager;

/** @class Collection
 *  @brief OpenBMC logging entry implementation.
 *  @details A concrete implementation for the
 *  xyz.openbmc_project.Collection and
 */
class Collection : public CollectionIfaces
{
    public:
        Collection() = delete;
        Collection(const Collection&) = delete;
        Collection& operator=(const Collection&) = delete;
        Collection(Collection&&) = delete;
        Collection& operator=(Collection&&) = delete;
        virtual ~Collection() = default;

        /** @brief Constructor to put object onto bus at a dbus path.
         *         Defer signal registration (pass true for deferSignal to the
         *         base class) until after the properties are set.
         *  @param[in] bus - Bus to attach to.
         *  @param[in] path - Path to attach at.
         *  @param[in] parent - The error's parent.
         */
        Collection(sdbusplus::bus::bus& bus,
              const std::string& path,
              Manager& parent) :
              CollectionIfaces(bus, path.c_str(), true),
              parent(parent){};

        /** @brief Delete all d-bus objects.
         */
        void delete_() override;

    private:
        /** @brief This entry's parent */
        Manager& parent;
};

} // namespace logging
} // namespace phosphor
