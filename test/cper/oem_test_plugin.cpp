#include <phosphor-logging/cper.hpp>

#include <span>
#include <string>

namespace v1 = phosphor::logging::cper::v1;

namespace
{

// Single-GUID plugin: registered for the memory section GUID of the
// memoryCper fixture (a5bc1114-6f64-4ede-b863-3e83ed7c83b1) and the fake
// OEM GUID of the fakeOemCper record (11111111-...).
const v1::Register single{
    std::vector<v1::Guid>{"a5bc1114-6f64-4ede-b863-3e83ed7c83b1",
                          "11111111-2222-3333-4444-555555555555"},
    [](v1::Type type, std::span<const std::uint8_t> raw, const v1::Oem& base,
       std::span<const v1::Guid> guids) -> v1::Entry {
        nlohmann::json value{
            {"type", static_cast<int>(type)},
            {"rawSize", raw.size()},
            {"baseKeys", base.size()},
            {"matched", guids.size()},
        };
        return {.key = "TestOem", .value = std::move(value)};
    }};

// Multi-GUID plugin: one handler serving two GUIDs, only one of which
// appears in any record, to exercise matched-subset dispatch.
const v1::Register multi{
    std::vector<v1::Guid>{"aaaaaaaa-0000-0000-0000-000000000000",
                          "bbbbbbbb-0000-0000-0000-000000000000"},
    [](v1::Type, std::span<const std::uint8_t>, const v1::Oem&,
       std::span<const v1::Guid> guids) -> v1::Entry {
        return {.key = "TestMulti",
                .value = {{"matched", std::string{guids.front()}}}};
    }};

} // namespace
