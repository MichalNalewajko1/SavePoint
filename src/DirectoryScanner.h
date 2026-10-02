#pragma once

#include "ScanSummary.h"

#include <filesystem>
#include <system_error>

class DirectoryScanner
{
public:
    explicit DirectoryScanner(bool useExclusions = true);

    ScanSummary scan(const std::filesystem::path &sourcePath, std::error_code &errorCode) const;

private:
    bool shouldSkipDirectory(const std::filesystem::path &directoryPath) const;

    bool applyExclusions;
};