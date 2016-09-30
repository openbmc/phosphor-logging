#pragma once

#include "elog-gen.hpp"

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
    dbus_commit(E.msgid); // call server
}

/** @fn elog()
 *  @brief Create a journal log entry based on predefined
 *          error log information
 *  @tparam E - Error log struct
 *  @param[in] args - Metadata fields to be added to the journal entry
 */
template <typename E, typename ...Args>
void elog(E&&... args)
{
    log<E.L>(msg(E.msg), std::forward<Args>(args)...);
}

/** @fn new_entry()
 *  @brief Create a metadata field
 *  @tparam M - Metadata struct
 *  @param[in] arg - Metadata value
 */
template <typename M, typename Arg>
auto new_entry(Arg&& arg)
{
    return entry(M.md_entry, arg); // defined in log.hpp
}

/** @fn prev_entry()
 *  @brief Retrieve a metadata field value from the journal
 *  @tparam M - Metadata struct
 */
template <typename M>
auto prev_entry()
{
    // TODO placeholder function call to retrieve most recent value
    // of specified metadata field
    result = find_meta(M.md_name); // call server
    return entry(M.md_entry, result);
}

} // namespace logging

} // namespace phosphor

