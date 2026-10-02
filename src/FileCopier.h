#pragma once

#include <filesystem>
#include <system_error>
#include <vector>

class FileCopier
{
public:
    FileCopier(const std::filesystem::path &source, const std::filesystem::path &destination);

    bool copy(const std::vector<std::filesystem::path> &files, std::error_code &errorCode) const;

private:
    std::filesystem::path sourcePath;
    std::filesystem::path destinationPath;
};