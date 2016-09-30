#pragma once

#include "elog-gen.hpp"
#include <utility>

namespace phosphor
{

namespace logging
{

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
 *  @tparam E - Error log struct
 *  @param[in] args - Metadata fields to be added to the journal entry
 */
template <typename E, typename ...Args>
void elog(E&& i_e, Args... i_args)
{
    phosphor::logging::log<i_e.L>(msg(i_e.msg), std::forward<Args>(i_args)...);
}

/** @fn new_entry()
 *  @brief Create a metadata field
 *  @tparam M - Metadata struct
 *  @param[in] arg - Metadata value
 */
template <typename M, typename Arg>
auto new_entry(M i_m, Arg&& arg)
{
    return entry(i_m.md_entry, arg); // defined in log.hpp
}

/** @fn prev_entry()
 *  @brief Retrieve a metadata field value from the journal
 *  @tparam M - Metadata struct
 */
template <typename M>
auto prev_entry(M i_m)
{
    // TODO placeholder function call to retrieve most recent value
    // of specified metadata field
    //result = find_meta(i_m.md_name); // call server
    //return entry(i_m.md_entry, result);
}

} // namespace logging

} // namespace phosphor

