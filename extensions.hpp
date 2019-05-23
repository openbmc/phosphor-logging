#pragma once

#include "elog_entry.hpp"

#include <functional>
#include <iostream>
#include <map>
#include <sdbusplus/bus.hpp>
#include <vector>

namespace phosphor
{
namespace logging
{
using DeleteList = std::vector<std::uint32_t>;
using StartupFunction = std::function<void(sdbusplus::bus::bus&)>;
using CreateFunction = std::function<DeleteList(const Entry&)>;
using DeleteFunction = std::function<void(std::uint32_t)>;
using DeleteProhibitedFunction = std::function<bool(std::uint32_t)>;

using StartupFunctions = std::vector<StartupFunction>;
using CreateFunctions = std::vector<CreateFunction>;
using DeleteFunctions = std::vector<DeleteFunction>;
using DeleteProhibitedFunctions = std::vector<DeleteProhibitedFunction>;

/**
 * @brief Register an extension hook function
 *
 * Call this macro at global scope to register a hook to call.
 * Each hook point has a unique function prototype.
 */
#define REGISTER_EXTENSION_FUNCTION(func)                                      \
    namespace func##_ns                                                        \
    {                                                                          \
        Extensions e{func};                                                    \
    }

/**
 * @brief Disable default error log capping
 *
 * Call this macro at global scope to tell phosphor-logging to disable its
 * default error log capping algorithm, so that an extension can use its own
 * instead.
 */
#define DISABLE_LOG_ENTRY_CAPS()                                               \
    namespace disable_caps##_ns                                                \
    {                                                                          \
        Extensions e{Extensions::DefaultErrorCaps::disable};                   \
    }

/**
 * @class Extensions
 *
 * This class manages any error log extensions.  Extensions can register
 * their hook functions with this class with the provided macros so that they
 * are then able to create their own types of logs based on the native logs.
 *
 * The class should only be constructed at a global scope via the macros.
 */
class Extensions
{
  public:
    Extensions() = delete;
    ~Extensions() = default;
    Extensions(const Extensions&) = delete;
    Extensions& operator=(const Extensions&) = delete;
    Extensions(Extensions&&) = delete;
    Extensions& operator=(Extensions&&) = delete;

    enum class DefaultErrorCaps
    {
        disable,
        enable
    };

    /**
     * @brief Constructor to register a startup function
     *
     * Functions registered with this contructor will be called
     * when phosphor-log-manager starts up.
     *
     * @param[in] func - The startup function to register
     */
    explicit Extensions(StartupFunction func)
    {
        startupFunctions.push_back(func);
    }

    /**
     * @brief Constructor to register a create function
     *
     * Functions registered with this contructor will be called
     * after phosphor-log-manager creates an event log.
     *
     * @param[in] func - The create function to register
     */
    explicit Extensions(CreateFunction func)
    {
        createFunctions.push_back(func);
    }

    /**
     * @brief Constructor to register a delete function
     *
     * Functions registered with this contructor will be called
     * after phosphor-log-manager deletes an event log.
     *
     * @param[in] func - The delete function to register
     */
    explicit Extensions(DeleteFunction func)
    {
        deleteFunctions.push_back(func);
    }

    /**
     * @brief Constructor to disable event log capping
     *
     * This constructor should only be called by the
     * DISABLE_LOG_ENTRY_CAPS macro to disable the default
     * event log capping so that the extension can use their own.
     *
     * @param[in] defaultCaps - Enable or disable default capping.
     */
    explicit Extensions(DefaultErrorCaps defaultCaps)
    {
        defaultErrorCaps = defaultCaps;
    }

    /**
     * @brief Returns the Startup functions
     * @return StartupFunctions - the Startup functions
     */
    static StartupFunctions& getStartupFunctions()
    {
        return startupFunctions;
    }

    /**
     * @brief Returns the Create functions
     * @return CreateFunctions - the Create functions
     */
    static CreateFunctions& getCreateFunctions()
    {
        return createFunctions;
    }

    /**
     * @brief Returns the Delete functions
     * @return DeleteFunctions - the Delete functions
     */
    static DeleteFunctions& getDeleteFunctions()
    {
        return deleteFunctions;
    }

    /**
     * @brief Returns the DeleteProhibited functions
     * @return DeleteProhibitedFunctions - the DeleteProhibited functions
     */
    static DeleteProhibitedFunctions& getDeleteProhibitedFunctions()
    {
        return deleteProhibitedFunctions;
    }

    /**
     * @brief Say if the default log capping policy should be disabled
     * @return bool - true if it should be disabled
     */
    static bool disableDefaultLogCaps()
    {
        return defaultErrorCaps == DefaultErrorCaps::disable;
    }

  private:
    /**
     * @brief Vector of functions to call on app startup.
     */
    static StartupFunctions startupFunctions;

    /**
     * @brief Vector of functions to call after creating an event log.
     */
    static CreateFunctions createFunctions;

    /**
     * @brief Vector of functions to call after deleting an event log.
     */
    static DeleteFunctions deleteFunctions;

    /**
     * @brief Vector of functions to call to check if deleting a
     *        particular event log is prohibited.
     */
    static DeleteProhibitedFunctions deleteProhibitedFunctions;

    /**
     * @brief If default log capping should be disabled.
     */
    static DefaultErrorCaps defaultErrorCaps;
};

} // namespace logging
} // namespace phosphor
