#include <fcntl.h>
#include <systemd/sd-bus.h>
#include <systemd/sd-event.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

class SystemdDebugFileMonitor
{
  private:
    static constexpr const char* STATE_FILE = "/run/systemd-debug-enabled";
    static constexpr uint64_t POLL_INTERVAL_USEC = 2000000; // 2 Second

    sd_bus* bus = nullptr;
    sd_event* event = nullptr;
    std::string last_log_level;
    bool startup_complete = false;

    void update_debug_state_file(bool enabled)
    {
        if (enabled)
        {
            int fd = open(STATE_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd >= 0)
            {
                close(fd);
                if (startup_complete)
                {
                    std::cout << "Debug mode enabled" << std::endl;
                }
            }
        }
        else
        {
            if (unlink(STATE_FILE) == 0 || errno == ENOENT)
            {
                if (startup_complete && errno != ENOENT)
                {
                    std::cout << "Debug mode disabled" << std::endl;
                }
            }
        }
    }

    std::string get_current_log_level()
    {
        sd_bus_error error = SD_BUS_ERROR_NULL;
        sd_bus_message* reply = NULL;
        std::string result;

        int r = sd_bus_get_property(
            bus, "org.freedesktop.systemd1", "/org/freedesktop/systemd1",
            "org.freedesktop.systemd1.Manager", "LogLevel", &error, &reply,
            "s");
        if (r < 0)
        {
            sd_bus_error_free(&error);
            sd_bus_message_unref(reply);
            return "";
        }

        const char* log_level = nullptr;
        r = sd_bus_message_read(reply, "s", &log_level);
        if (r >= 0 && log_level)
        {
            result = log_level;
        }

        sd_bus_error_free(&error);
        sd_bus_message_unref(reply);
        return result;
    }

    void check_and_update()
    {
        std::string current_level = get_current_log_level();
        if (current_level.empty())
        {
            return;
        }

        if (current_level != last_log_level)
        {
            std::cout << "LogLevel changed: " << last_log_level << " -> "
                      << current_level << std::endl;

            bool is_debug = (current_level == "debug");
            update_debug_state_file(is_debug);
            last_log_level = current_level;
        }
    }

    static int on_timer(sd_event_source* s, uint64_t usec, void* userdata)
    {
        auto* self = static_cast<SystemdDebugFileMonitor*>(userdata);

        self->check_and_update();

        uint64_t next = usec + POLL_INTERVAL_USEC;
        sd_event_source_set_time(s, next);

        return 0;
    }

  public:
    int run()
    {
        sd_event_source* timer = nullptr;

        auto cleanup = [&]() {
            if (timer)
                sd_event_source_unref(timer);
            if (event)
                sd_event_unref(event);
            if (bus)
                sd_bus_unref(bus);
        };

        // Connect to Dbus
        int r = sd_bus_open_system(&bus);
        if (r < 0)
        {
            std::cerr << "Failed to connect to system bus" << std::endl;
            cleanup();
            return -1;
        }

        // Get initial status
        last_log_level = get_current_log_level();
        if (last_log_level.empty())
        {
            std::cerr << "Failed to get initial LogLevel" << std::endl;
            cleanup();
            return -1;
        }

        bool is_debug = (last_log_level == "debug");
        update_debug_state_file(is_debug);

        // Create event loop
        r = sd_event_default(&event);
        if (r < 0)
        {
            std::cerr << "Failed to create event loop" << std::endl;
            cleanup();
            return -1;
        }

        // Set timer
        uint64_t now;
        r = sd_event_now(event, CLOCK_MONOTONIC, &now);
        if (r < 0)
        {
            std::cerr << "Failed to get current time" << std::endl;
            cleanup();
            return -1;
        }

        r = sd_event_add_time(event, &timer, CLOCK_MONOTONIC,
                              now + POLL_INTERVAL_USEC, 1000000, on_timer,
                              this);
        if (r < 0)
        {
            std::cerr << "Failed to create timer" << std::endl;
            cleanup();
            return -1;
        }

        // Keep monitor
        r = sd_event_source_set_enabled(timer, SD_EVENT_ON);
        if (r < 0)
        {
            std::cerr << "Failed to enable timer" << std::endl;
            cleanup();
            return -1;
        }

        startup_complete = true;
        std::cout << "SystemD debug monitor started (LogLevel: "
                  << last_log_level << ")" << std::endl;

        r = sd_event_loop(event);

        cleanup();
        return r;
    }
};

int main()
{
    std::cerr.setf(std::ios::unitbuf);

    SystemdDebugFileMonitor monitor;
    return monitor.run();
}
