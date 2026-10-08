#include <phosphor-logging/lg2.hpp>
#include <phosphor-logging/lg2/flags.hpp>
#include <phosphor-logging/lg2/header.hpp>

#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

#include <gtest/gtest.h>

// Tests for log_flag bit operations and values in baseline lg2
TEST(Lg2FlagsTest, BaselineFlags)
{
    // Verify bitwise OR combinations
    constexpr auto combined = lg2::hex | lg2::field16;
    static_assert((combined.value & lg2::hex.value) == lg2::hex.value);
    static_assert((combined.value & lg2::field16.value) == lg2::field16.value);

    EXPECT_NE(lg2::hex.value, 0u);
    EXPECT_NE(lg2::field16.value, 0u);
    EXPECT_EQ(combined.value, lg2::hex.value | lg2::field16.value);
}

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
    static_assert(lg2::details::is_log_flag_v<decltype(lg2::str)>);

    // Compound flags
    static_assert(
        lg2::details::is_log_flag_v<decltype(lg2::hex | lg2::field16)>);
    static_assert(lg2::details::is_log_flag_v<decltype(lg2::dec | lg2::field32 |
                                                       lg2::signed_val)>);

    // References and const-qualifications
    static_assert(lg2::details::is_log_flag<decltype(lg2::hex)>::value);
    static_assert(lg2::details::is_log_flag<const decltype(lg2::hex)&>::value);
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

// Tests for header_str compile-time validation and header_str_conversion_t
TEST(Lg2HeaderTest, BaselineHeaders)
{
    // Valid headers
    constexpr lg2::details::header_str h1 = "VALID_HEADER";
    static_assert(h1.value == "VALID_HEADER");

    constexpr lg2::details::header_str h2 = "COUNT123";
    static_assert(h2.value == "COUNT123");

    static constexpr char h3_arr[] = "ARRAY_HEADER";
    constexpr lg2::details::header_str h3 = h3_arr;
    static_assert(h3.value == "ARRAY_HEADER");

    EXPECT_STREQ(h1.data(), "VALID_HEADER");
    EXPECT_STREQ(static_cast<const char*>(h1), "VALID_HEADER");
    EXPECT_STREQ(h3.data(), "ARRAY_HEADER");

    // header_str_conversion_t: non-strings are left unchanged
    static_assert(
        std::is_same_v<lg2::details::header_str_conversion_t<int>, int>);
    static_assert(
        std::is_same_v<lg2::details::header_str_conversion_t<double>, double>);
    static_assert(
        std::is_same_v<lg2::details::header_str_conversion_t<std::string>,
                       std::string>);

    // header_str_conversion_t: string literals are converted to header_str
    static_assert(
        std::is_same_v<lg2::details::header_str_conversion_t<const char[13]>,
                       lg2::details::header_str>);

    // header_str can only be constructed from string literal arrays, not raw
    // pointers or string_views (preventing non-null-terminated buffer reads)
    static_assert(
        !std::is_constructible_v<lg2::details::header_str, const char*>);
    static_assert(
        !std::is_constructible_v<lg2::details::header_str, std::string_view>);
    static_assert(
        !std::is_constructible_v<lg2::details::header_str, std::string>);
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
                                 lg2::details::header_str>);

    // Case 4: Trailing Header + Flag without a value
    using DanglingHeaderFlagTuple =
        lg2::details::convert_args_t<const char[5], decltype(lg2::hex)>;
    static_assert(std::tuple_size_v<DanglingHeaderFlagTuple> == 2);
    static_assert(
        std::is_same_v<std::tuple_element_t<0, DanglingHeaderFlagTuple>,
                       lg2::details::header_str>);
    static_assert(
        std::is_same_v<std::tuple_element_t<1, DanglingHeaderFlagTuple>,
                       decltype(lg2::hex)>);

    // Case 2: Header + Value (numeric)
    using HeaderValueTuple = lg2::details::convert_args_t<const char[4], int>;
    static_assert(std::tuple_size_v<HeaderValueTuple> == 2);
    static_assert(std::is_same_v<std::tuple_element_t<0, HeaderValueTuple>,
                                 lg2::details::header_str>);
    static_assert(
        std::is_same_v<std::tuple_element_t<1, HeaderValueTuple>, int>);

    // Case 2: Header + String literal value
    // Verify that the string literal value is preserved and NOT converted into
    // header_str
    using HeaderStrValTuple =
        lg2::details::convert_args_t<const char[4], const char[12]>;
    static_assert(std::tuple_size_v<HeaderStrValTuple> == 2);
    static_assert(std::is_same_v<std::tuple_element_t<0, HeaderStrValTuple>,
                                 lg2::details::header_str>);
    static_assert(std::is_same_v<std::tuple_element_t<1, HeaderStrValTuple>,
                                 const char[12]>);
    static_assert(!std::is_same_v<std::tuple_element_t<1, HeaderStrValTuple>,
                                  lg2::details::header_str>);

    // Case 1: Header + Flag + Value
    using HeaderFlagValueTuple =
        lg2::details::convert_args_t<const char[4], decltype(lg2::hex),
                                     unsigned int>;
    static_assert(std::tuple_size_v<HeaderFlagValueTuple> == 3);
    static_assert(std::is_same_v<std::tuple_element_t<0, HeaderFlagValueTuple>,
                                 lg2::details::header_str>);
    static_assert(std::is_same_v<std::tuple_element_t<1, HeaderFlagValueTuple>,
                                 decltype(lg2::hex)>);
    static_assert(std::is_same_v<std::tuple_element_t<2, HeaderFlagValueTuple>,
                                 unsigned int>);

    // Case 1: Header + Flag + String literal value
    using HeaderFlagStrValTuple =
        lg2::details::convert_args_t<const char[4], decltype(lg2::str),
                                     const char[12]>;
    static_assert(std::tuple_size_v<HeaderFlagStrValTuple> == 3);
    static_assert(std::is_same_v<std::tuple_element_t<0, HeaderFlagStrValTuple>,
                                 lg2::details::header_str>);
    static_assert(std::is_same_v<std::tuple_element_t<1, HeaderFlagStrValTuple>,
                                 decltype(lg2::str)>);
    static_assert(std::is_same_v<std::tuple_element_t<2, HeaderFlagStrValTuple>,
                                 const char[12]>);
    static_assert(
        !std::is_same_v<std::tuple_element_t<2, HeaderFlagStrValTuple>,
                        lg2::details::header_str>);

    // Case 1: Header + Compound Flag + Value
    using HeaderCompoundFlagValueTuple =
        lg2::details::convert_args_t<const char[4],
                                     decltype(lg2::hex | lg2::field16),
                                     unsigned int>;
    static_assert(std::tuple_size_v<HeaderCompoundFlagValueTuple> == 3);
    static_assert(
        std::is_same_v<std::tuple_element_t<0, HeaderCompoundFlagValueTuple>,
                       lg2::details::header_str>);
    static_assert(
        std::is_same_v<std::tuple_element_t<1, HeaderCompoundFlagValueTuple>,
                       decltype(lg2::hex | lg2::field16)>);
    static_assert(
        std::is_same_v<std::tuple_element_t<2, HeaderCompoundFlagValueTuple>,
                       unsigned int>);

    // Multiple argument sets: {Header, Value} and {Header, Flag, Value}
    using MultiArgsTuple =
        lg2::details::convert_args_t<const char[4], const char[6],
                                     const char[4], decltype(lg2::hex), int>;
    static_assert(std::tuple_size_v<MultiArgsTuple> == 5);
    static_assert(std::is_same_v<std::tuple_element_t<0, MultiArgsTuple>,
                                 lg2::details::header_str>);
    static_assert(
        std::is_same_v<std::tuple_element_t<1, MultiArgsTuple>, const char[6]>);
    static_assert(std::is_same_v<std::tuple_element_t<2, MultiArgsTuple>,
                                 lg2::details::header_str>);
    static_assert(std::is_same_v<std::tuple_element_t<3, MultiArgsTuple>,
                                 decltype(lg2::hex)>);
    static_assert(std::is_same_v<std::tuple_element_t<4, MultiArgsTuple>, int>);
}

// Tests for basic lg2 logging functionality that work with baseline lg2
TEST(Lg2LogTest, BasicLogging)
{
    using namespace std::string_literals;

    EXPECT_NO_THROW(lg2::error("Message without fields"));

    EXPECT_NO_THROW(
        lg2::debug("Debug with hex flag: {VAL}", "VAL", lg2::hex, 0xABCD));

    EXPECT_NO_THROW(lg2::debug("Debug with compound flags: {VAL}", "VAL",
                               lg2::hex | lg2::field16, 0x1234u));

    EXPECT_NO_THROW(
        lg2::info("Message with std::string: {STR}", "STR", "a string"s));
}

// Verify that string_view values are converted into a NUL-terminated string
// that holds only the characters covered by the view.
TEST(Lg2LogTest, StringViewNotNulTerminated)
{
    constexpr char buf[] = {'a', 'b', 'c', 'X', 'Y', 'Z'};
    const std::string_view sv{buf, 3};

    auto t = lg2::details::log_convert("STR", lg2::details::log_flag<>{}, sv);
    static_assert(
        std::is_same_v<std::tuple_element_t<2, decltype(t)>, std::string>);
    EXPECT_EQ(std::get<1>(t), lg2::str.value);
    EXPECT_EQ(std::get<2>(t), "abc");
    EXPECT_STREQ(std::get<2>(t).c_str(), "abc");

    EXPECT_NO_THROW(lg2::info("string_view: {STR}", "STR", sv));
}

// Tests for lg2 logging with string literals and flags
TEST(Lg2LogTest, LogWithStringLiteralsAndFlags)
{
    // String literal values containing lowercase letters and spaces
    // Prior to convert_args_t, string literal values were incorrectly converted
    // to header_str, failing compile-time journald header validation.
    EXPECT_NO_THROW(lg2::info("Simple message: {STR}", "STR",
                              "lowercase value with spaces"));

    // Uppercase string literal value (previously converted to header_str and
    // rejected by static_assert in log_conversion::step)
    EXPECT_NO_THROW(lg2::info("Status: {STATUS}", "STATUS", "READY"));

    // Empty string literal value (previously rejected by header_str non-zero
    // length check)
    EXPECT_NO_THROW(lg2::info("Empty value: {EMPTY}", "EMPTY", ""));

    // Explicit lg2::str flag with string literal value (Case 1 triplet)
    EXPECT_NO_THROW(lg2::info("Flagged string: {STR}", "STR", lg2::str,
                              "flagged literal value"));

    EXPECT_NO_THROW(
        lg2::warning("Multi args: {STR} {HEX}", "STR",
                     "another lowercase string", "HEX", lg2::hex, 1234));

    // Compound flags with string literal value
    EXPECT_NO_THROW(
        lg2::warning("Compound flags: {STR} {HEX}", "STR", "READY", "HEX",
                     lg2::hex | lg2::field16, 0x12u));

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
