#include "file.hpp"

#include <filesystem>
#include <fstream>
#include <regex>

namespace phosphor
{
namespace audit
{

namespace fs = std::filesystem;

void FileHandler::responseAction(const std::string& buffer)
{
    writeFile(buffer);
}

std::string FileHandler::getFilePath(size_t len)
{
    fs::path dir{filePath};
    if (!fs::exists(dir) || fs::is_empty(dir))
    {
        if (!fs::create_directories(dir))
        {

            return auditPath;
        }
    }

    std::regex fileSuffix("^audit.log(.[1-9])*");
    std::vector<fs::path> vec;
    for (auto& iter : fs::directory_iterator(dir))
    {
        if (!fs::is_regular_file(iter))
        {
            continue;
        }

        auto filename = iter.path().filename();
        if (!std::regex_match(filename.string(), fileSuffix))
        {
            continue;
        }

        vec.push_back(iter);
    }

    if (vec.empty())
    {
        return auditPath;
    }

    std::sort(vec.begin(), vec.end());
    auto dirPath = vec[vec.size() - 1];
    std::uintmax_t size = std::filesystem::file_size(dirPath);

    if (size + len > maxSize)
    {
        // creat a new file for audit log
        std::string ext = dirPath.extension();
        int index = std::atoi(ext.substr(1).c_str());
        std::string newPath{};
        newPath.append(filePath);
        newPath.append("/");
        newPath.append(fileHeader);
        newPath.append(".");
        newPath.append(std::to_string(++index));

        return newPath;
    }

    return dirPath;
}

int FileHandler::writeFile(const std::string& buffer)
{
    fs::path path = getFilePath(buffer.size());

    if (fs::exists(path) && !fs::is_regular_file(path))
    {
        fs::remove(path);
    }

    std::ofstream ofs(path, std::ios::app);
    ofs << buffer;
    ofs.close();

    return 0;
}

} // namespace audit
} // namespace phosphor