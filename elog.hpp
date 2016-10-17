#pragma once

#include <tuple>
#include "elog-gen.hpp"
#include <utility>

namespace phosphor
{

namespace logging
{

class elogException : public std::exception
{
public:

    std::string err_code;
    std::string err_msg;

    elogException (const std::string& i_errCode, const std::string& i_errMsg) :
        err_code(i_errCode), err_msg(i_errMsg) {}

    virtual const char* what()
    {
        return err_code.c_str();
    }
};

/** @fn commit()
 *  @brief Create an error log entry based on journal
 *          entry with a specified MSG_ID
 *  @tparam E - Error log struct
 */
template <typename E>
void commit()
{
    // TODO placeholder function call
    // to call the new error log server to create
    // an error log on the BMC flash
    // dbus_commit(E.msgid); // call server
}

/** @fn elog()
 *  @brief Create a journal log entry based on predefined
 *          error log information
 *  @tparam T - Error log type
 *  @param[in] i_args - Metadata fields to be added to the journal entry
 */
template <typename T, typename ...Args>
void elog(Args... i_args)
{
    // Validate the caller passed in the required parameters
    static_assert(std::is_same<typename T::metadata_types,
                               std::tuple<std::decay_t<Args>...>>
                               ::value,
                  "You are not passing in required arguments for this error");

    // Send metadata over to log interface
    log<T::L>(msg(T::err_msg),
              std::forward<typename Args::type>(i_args._entry)...);

    // Now throw an exception for this error
    throw elogException(T::err_code, T::err_msg);
}

} // namespace logging

} // namespace phosphor

