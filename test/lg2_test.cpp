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
