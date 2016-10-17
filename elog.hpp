#pragma once

#include <tuple>
#include <utility>
#include "elog-gen.hpp"

namespace phosphor
{

namespace logging
{

// The prev_entry and deduce_entry_type structures below are used
// to verify at compile time the required parameters have been passed to
// the elog interface and then to also be able to forward the empty
// prev_entry rvalue instances on to the log interface (which will
// ignore them).
template <typename T>
struct prev_entry
{
    using type = T;
};

template <typename T>
struct deduce_entry_type
{

    using type = T;
    auto get() { return value._entry; }

    T value;
};

template <typename T>
struct deduce_entry_type<prev_entry<T>>
{
    using type = T;
    auto get() { return std::make_tuple(); }

    prev_entry<T> value;
};

template <typename T> using deduce_entry_type_t =
        typename deduce_entry_type<T>::type;


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
                                       std::tuple<deduce_entry_type_t<Args>...>>
                                       ::value,
                  "You are not passing in required arguments for this error");

    log<T::L>(T::err_msg,
              deduce_entry_type<Args>{i_args}.get()...);

    // Now throw an exception for this error
    throw elogException(T::err_code, T::err_msg);
}

} // namespace logging

} // namespace phosphor

