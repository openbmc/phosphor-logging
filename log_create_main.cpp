#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>
#include <phosphor-logging/commit.hpp>
#include <sdbusplus/exception.hpp>

// We don't actually use the Logging events, but we need to include the
// header in order to force linking against the PDI library.
#include <xyz/openbmc_project/Logging/event.hpp>

#include <iostream>
#include <string>

void list_all()
{
    std::cout << "Known events:" << std::endl;
    for (const auto& e : sdbusplus::exception::known_events())
    {
        std::cout << "    " << e << std::endl;
    }
}

int generate_event(const std::string& event_id, const nlohmann::json& data)
{
    if (event_id.empty())
    {
        std::cerr << "event required" << std::endl;
        return 1;
    }

    nlohmann::json j = {{event_id, data}};

    try
    {
        sdbusplus::exception::throw_via_json(j);
    }
    catch (sdbusplus::exception::generated_event_base& e)
    {
        auto path = lg2::details::commit(std::move(e));
        std::cout << path.str << std::endl;
        return 0;
    }

    std::cerr << "Unknown event: " << event_id << std::endl;
    return 1;
}

int main(int argc, char** argv)
{
    CLI::App app{"log-create"};

    std::string json_str;
    app.add_option("-j,--json", json_str, "Event data as a JSON object")
        ->default_val("{}");

    std::string event{};
    auto event_option = app.add_option("event", event, "Event name");

    bool list_only;
    app.add_flag("-l,--list", list_only, "List all events")
        ->excludes(event_option);

    CLI11_PARSE(app, argc, argv);

    if (list_only)
    {
        list_all();
        return 0;
    }

    return generate_event(event, nlohmann::json::parse(json_str));
}
