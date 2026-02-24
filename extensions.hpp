#pragma once

#include "elog_entry.hpp"
#include "log_manager.hpp"

#include <functional>
#include <vector>

namespace phosphor
{
namespace logging
{

/**
 * @brief The function type that will be called on start up.
 * @param[in] internal::Manager& - A reference to the Manager class.
 */
using StartupFunction = std::function<void(internal::Manager&)>;

using AdditionalDataArg = std::map<std::string, std::string>;
using AssociationEndpointsArg = std::vector<std::string>;
using FFDCArg = FFDCEntries;

/**
 *  @brief The function type that will be called after an event log
 *         is created.
 * @param[in] const std::string& - The Message property
 * @param[in] uin32_t - The event log ID
 * @param[in] uint64_t - The event log timestamp
 * @param[in] Level - The event level
 * @param[in] const AdditionalDataArg&) - the additional data
 * @param[in] const AssociationEndpoints& - Association endpoints (callouts)
 * @param[in] const FFDCArg& - A vector of FFDC file info.
 */
using CreateFunction = std::function<void(
    const std::string&, uint32_t, uint64_t, Entry::Level,
    const AdditionalDataArg&, const AssociationEndpointsArg&, const FFDCArg&)>;

/**
 * @brief The function type that will be called after an event log is deleted.
 * @param[in] uint32_t - The event log ID
 */
using DeleteFunction = std::function<void(uint32_t)>;

/**
 * @brief The function type that will to check if an event log is prohibited
 *        from being deleted.
 *        The same function is used to check if an event log is prohibited from
 *        setting Resolved flag, as Resolve is prohibited as long as Delete is
 *        prohibited.
 *
 * @param[in] uint32_t - The event log ID
 * @param[out] bool - set to true if the delete is prohibited
 */
using DeleteProhibitedFunction = std::function<void(uint32_t, bool&)>;

/**
 * @brief The function type that will return list of Hw Isolated
 *        log IDs
 * @param[out] std::vector<uint32_t>& - Hw Isolated log IDs
 */
using LogIDWithHwIsolationFunction =
    std::function<void(std::vector<uint32_t>&)>;
using LogIDsWithHwIsolationFunctions =
    std::vector<LogIDWithHwIsolationFunction>;

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
     * Functions registered with this constructor will be called
     * when phosphor-log-manager starts up.
     *
     * @param[in] func - The startup function to register
     */
    explicit Extensions(StartupFunction func)
    {
        getStartupFunctions().push_back(func);
    }

    /**
     * @brief Constructor to register a create function
     *
     * Functions registered with this constructor will be called
     * after phosphor-log-manager creates an event log.
     *
     * @param[in] func - The create function to register
     */
    explicit Extensions(CreateFunction func)
    {
        getCreateFunctions().push_back(func);
    }

    /**
     * @brief Constructor to register a delete function
     *
     * Functions registered with this constructor will be called
     * after phosphor-log-manager deletes an event log.
     *
     * @param[in] func - The delete function to register
     */
    explicit Extensions(DeleteFunction func)
    {
        getDeleteFunctions().push_back(func);
    }

    /**
     * @brief Constructor to register a delete prohibition function
     *
     * Functions registered with this constructor will be called
     * before phosphor-log-manager deletes an event log to ensure
     * deleting the log is allowed.
     *
     * @param[in] func - The function to register
     */
    explicit Extensions(DeleteProhibitedFunction func)
    {
        getDeleteProhibitedFunctions().push_back(func);
    }

    /**
     * @brief Constructor to register a LogID with HwIsolation function
     *
     * Functions registered with this constructor will be called
     * before phosphor-log-manager deletes all event log.
     *
     * @param[in] func - The function to register
     */
    explicit Extensions(LogIDWithHwIsolationFunction func)
    {
        getLogIDWithHwIsolationFunctions().emplace_back(func);
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
        getDefaultErrorCaps() = defaultCaps;
    }

    /**
     * @brief Returns the Startup functions
     * @return StartupFunctions - the Startup functions
     */
    static StartupFunctions& getStartupFunctions();

    /**
     * @brief Returns the Create functions
     * @return CreateFunctions - the Create functions
     */
    static CreateFunctions& getCreateFunctions();

    /**
     * @brief Returns the Delete functions
     * @return DeleteFunctions - the Delete functions
     */
    static DeleteFunctions& getDeleteFunctions();

    /**
     * @brief Returns the DeleteProhibited functions
     * @return DeleteProhibitedFunctions - the DeleteProhibited functions
     */
    static DeleteProhibitedFunctions& getDeleteProhibitedFunctions();

    /**
     * @brief Returns the LogIDWithHwIsolationFunction functions
     * @return LogIDsWithHwIsolationFunctions - the LogIDWithHwIsolationFunction
     * functions
     */
    static LogIDsWithHwIsolationFunctions& getLogIDWithHwIsolationFunctions();

    /**
     * @brief Returns the DefaultErrorCaps value
     * @return DefaultErrorCaps - the DefaultErrorCaps value
     */
    static DefaultErrorCaps& getDefaultErrorCaps();

    /**
     * @brief Say if the default log capping policy should be disabled
     * @return bool - true if it should be disabled
     */
    static bool disableDefaultLogCaps()
    {
        return getDefaultErrorCaps() == DefaultErrorCaps::disable;
    }
};

} // namespace logging
} // namespace phosphor
