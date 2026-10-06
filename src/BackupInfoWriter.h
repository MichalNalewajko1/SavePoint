#pragma once

#include "ScanSummary.h"
#include <filesystem>

class BackupInfoWriter
{
public:
    explicit BackupInfoWriter(const std::filesystem::path &versionPath);

    bool write(const std::filesystem::path &sourcePath, const ScanSummary &summary) const;

private:
    std::filesystem::path backupVersionPath;
};