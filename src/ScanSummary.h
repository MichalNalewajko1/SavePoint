#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

struct ScanSummary
{
    std::vector<std::filesystem::path> files;
    std::vector<std::filesystem::path> directories;
    std::uintmax_t totalSize = 0;
};