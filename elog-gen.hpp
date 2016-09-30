#include <vector>
#include <string>
#include <log.hpp>

namespace phosphor
{

namespace logging
{

typedef struct
{
    std::string md_name;
    std::string md_entry;
} md;

struct FILE_NOT_FOUND_ERROR
{
    std::string exc_msg = "org.freedesktop.FILE_NOT_FOUND_ERROR";
    std::string msg = "File was not found";
    std::string msgid = "0fbc349a-4ad9-4657-9cd7-17bba1e18964";
    level L = level::INFO;
    std::vector<std::string> META_LIST {"ERRNO", "FILE_PATH", "FILE_NAME", };
};

md ERRNO {"ERRNO", "ERRNO=%d"};
md FILE_PATH {"FILE_PATH", "FILE_PATH=%s"};
md FILE_NAME {"FILE_NAME", "FILE_NAME=%s"};

struct GETSCOM_ERROR
{
    std::string exc_msg = "org.freedesktop.GETSCOM_ERROR";
    std::string msg = "Getscom call failed";
    std::string msgid = "ff3c3acc-4a16-4722-813e-9064f9a8405d";
    level L = level::ERR;
    std::vector<std::string> META_LIST {"DEV_ADDR", "DEV_ID", "DEV_NAME", };
};

md DEV_ADDR {"DEV_ADDR", "DEV_ADDR=%d"};
md DEV_ID {"DEV_ID", "DEV_ID=%d"};
md DEV_NAME {"DEV_NAME", "DEV_NAME=%s"};

} // namespace logging

} // namespace phosphor
