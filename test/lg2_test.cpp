#include <phosphor-logging/lg2.hpp>
#include <phosphor-logging/lg2/flags.hpp>
#include <phosphor-logging/lg2/header.hpp>

#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

#include <gtest/gtest.h>

// Tests for is_log_flag type trait
TEST(Lg2FlagsTest, IsLogFlag)
{
    // Basic log flags
    static_assert(lg2::details::is_log_flag_v<decltype(lg2::hex)>);
    static_assert(lg2::details::is_log_flag_v<decltype(lg2::dec)>);
    static_assert(lg2::details::is_log_flag_v<decltype(lg2::bin)>);
    static_assert(lg2::details::is_log_flag_v<decltype(lg2::floating)>);
    static_assert(lg2::details::is_log_flag_v<decltype(lg2::signed_val)>);
    static_assert(lg2::details::is_log_flag_v<decltype(lg2::unsigned_val)>);
    static_assert(lg2::details::is_log_flag_v<decltype(lg2::field8)>);
    static_assert(lg2::details::is_log_flag_v<decltype(lg2::field16)>);
    static_assert(lg2::details::is_log_flag_v<decltype(lg2::field32)>);
    static_assert(lg2::details::is_log_flag_v<decltype(lg2::field64)>);

    // Compound flags
    static_assert(
        lg2::details::is_log_flag_v<decltype(lg2::hex | lg2::field16)>);
    static_assert(lg2::details::is_log_flag_v<decltype(lg2::dec | lg2::field32 |
                                                       lg2::signed_val)>);

    // References and const-qualifications
    static_assert(lg2::details::is_log_flag_v<const decltype(lg2::hex)&>);
    static_assert(lg2::details::is_log_flag_v<decltype(lg2::hex)&&>);

    // Non-flags
    static_assert(!lg2::details::is_log_flag_v<int>);
    static_assert(!lg2::details::is_log_flag_v<const char*>);
    static_assert(!lg2::details::is_log_flag_v<char[5]>);
    static_assert(!lg2::details::is_log_flag_v<std::string>);
    static_assert(!lg2::details::is_log_flag_v<std::string_view>);
    static_assert(!lg2::details::is_log_flag_v<double>);

    EXPECT_TRUE(lg2::details::is_log_flag_v<decltype(lg2::hex)>);
    EXPECT_FALSE(lg2::details::is_log_flag_v<int>);
    EXPECT_FALSE(lg2::details::is_log_flag_v<const char*>);
}

// Tests for convert_args_t template pack conversion
TEST(Lg2HeaderTest, ConvertArgs)
{
    // Empty argument pack
    using EmptyTuple = lg2::details::convert_args_t<>;
    static_assert(std::is_same_v<EmptyTuple, std::tuple<>>);

    // Single argument (Case 3: dangling header)
    using SingleArgTuple = lg2::details::convert_args_t<const char[5]>;
    static_assert(std::tuple_size_v<SingleArgTuple> == 1);
    static_assert(std::is_same_v<std::tuple_element_t<0, SingleArgTuple>,
                                 const lg2::details::header_str&>);

    // Case 2: Header + Value (numeric)
    using HeaderValueTuple = lg2::details::convert_args_t<const char[4], int>;
    static_assert(std::tuple_size_v<HeaderValueTuple> == 2);
    static_assert(std::is_same_v<std::tuple_element_t<0, HeaderValueTuple>,
                                 const lg2::details::header_str&>);
    static_assert(
        std::is_same_v<std::tuple_element_t<1, HeaderValueTuple>, int>);

    // Case 2: Header + String literal value
    // Verify that the string literal value is preserved and NOT converted into
    // header_str
    using HeaderStrValTuple =
        lg2::details::convert_args_t<const char[4], const char[12]>;
    static_assert(std::tuple_size_v<HeaderStrValTuple> == 2);
    static_assert(std::is_same_v<std::tuple_element_t<0, HeaderStrValTuple>,
                                 const lg2::details::header_str&>);
    static_assert(std::is_same_v<std::tuple_element_t<1, HeaderStrValTuple>,
                                 const char[12]>);
    static_assert(!std::is_same_v<std::tuple_element_t<1, HeaderStrValTuple>,
                                  const lg2::details::header_str&>);

    // Case 1: Header + Flag + Value
    using HeaderFlagValueTuple =
        lg2::details::convert_args_t<const char[4], decltype(lg2::hex),
                                     unsigned int>;
    static_assert(std::tuple_size_v<HeaderFlagValueTuple> == 3);
    static_assert(std::is_same_v<std::tuple_element_t<0, HeaderFlagValueTuple>,
                                 const lg2::details::header_str&>);
    static_assert(std::is_same_v<std::tuple_element_t<1, HeaderFlagValueTuple>,
                                 decltype(lg2::hex)>);
    static_assert(std::is_same_v<std::tuple_element_t<2, HeaderFlagValueTuple>,
                                 unsigned int>);

    // Multiple argument sets: {Header, Value} and {Header, Flag, Value}
    using MultiArgsTuple =
        lg2::details::convert_args_t<const char[4], const char[6],
                                     const char[4], decltype(lg2::hex), int>;
    static_assert(std::tuple_size_v<MultiArgsTuple> == 5);
    static_assert(std::is_same_v<std::tuple_element_t<0, MultiArgsTuple>,
                                 const lg2::details::header_str&>);
    static_assert(
        std::is_same_v<std::tuple_element_t<1, MultiArgsTuple>, const char[6]>);
    static_assert(std::is_same_v<std::tuple_element_t<2, MultiArgsTuple>,
                                 const lg2::details::header_str&>);
    static_assert(std::is_same_v<std::tuple_element_t<3, MultiArgsTuple>,
                                 decltype(lg2::hex)>);
    static_assert(std::is_same_v<std::tuple_element_t<4, MultiArgsTuple>, int>);
}

// Tests for lg2 logging with string literals and flags
TEST(Lg2LogTest, LogWithStringLiteralsAndFlags)
{
    // String literal values containing lowercase letters and spaces
    // Prior to convert_args_t, string literal values were incorrectly converted
    // to header_str, failing compile-time journald header validation.
    EXPECT_NO_THROW(lg2::info("Simple message: {STR}", "STR",
                              "lowercase value with spaces"));

    EXPECT_NO_THROW(
        lg2::debug("Debug with hex flag: {VAL}", "VAL", lg2::hex, 0xABCD));

    EXPECT_NO_THROW(
        lg2::warning("Multi args: {STR} {HEX}", "STR",
                     "another lowercase string", "HEX", lg2::hex, 1234));

    EXPECT_NO_THROW(lg2::error("Message without fields"));

    // Explicit source_location
    EXPECT_NO_THROW(
        lg2::log(std::source_location::current(), "Custom location: {MSG}",
                 "MSG", "custom location value"));

    // lg2::log using the new deduction guide
    EXPECT_NO_THROW(lg2::log("Base log with deduction guide: {STR}", "STR",
                             "base log message"));

    // Level alias
    EXPECT_NO_THROW(
        lg2::notice("Notice level: {STR}", "STR", "notice message"));
}
