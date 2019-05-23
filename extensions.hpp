#pragma once

#include "elog_entry.hpp"

#include <functional>
#include <iostream>
#include <map>
#include <sdbusplus/bus.hpp>
#include <string>
#include <vector>

namespace phosphor
{
namespace logging
{
using DeleteList = std::vector<std::uint32_t>;
using StartupFunction = std::function<void(sdbusplus::bus::bus&)>;
using CreateFunction =
    std::function<DeleteList(const Entry&, const std::string&)>;
using DeleteFunction = std::function<void(std::uint32_t)>;

using StartupFunctions = std::vector<StartupFunction>;
using CreateFunctions = std::vector<CreateFunction>;
using DeleteFunctions = std::vector<DeleteFunction>;

#define REGISTER_EXTENSION_FUNCTION(func)                                      \
    namespace func##_ns                                                        \
    {                                                                          \
        Extensions r{func};                                                    \
    }

#define DISABLE_LOG_ENTRY_CAPS()                                               \
    namespace disable_caps##_ns                                                \
    {                                                                          \
        Extensions r{Extensions::DefaultErrorCaps::disable};                   \
    }

class Extensions
{
  public:
    enum class DefaultErrorCaps
    {
        disable,
        enable
    };
    explicit Extensions(StartupFunction func)
    {
        startupFunctions.push_back(func);
    }

    explicit Extensions(CreateFunction func)
    {
        createFunctions.push_back(func);
    }

    explicit Extensions(DeleteFunction func)
    {
        deleteFunctions.push_back(func);
    }

    //Fails horribly if this is just a bool
    explicit Extensions(DefaultErrorCaps defaultCaps)
    {
        defaultErrorCaps = defaultCaps;
    }

    static StartupFunctions& getStartupFunctions()
    {
        return startupFunctions;
    }

    static CreateFunctions& getCreateFunctions()
    {
        return createFunctions;
    }

    static DeleteFunctions& getDeleteFunctions()
    {
        return deleteFunctions;
    }

    static bool disableDefaultLogCaps()
    {
        return defaultErrorCaps == DefaultErrorCaps::disable;
    }

  private:
    static StartupFunctions startupFunctions;
    static CreateFunctions createFunctions;
    static DeleteFunctions deleteFunctions;
    static DefaultErrorCaps defaultErrorCaps;
};

} // namespace logging
} // namespace phosphor
