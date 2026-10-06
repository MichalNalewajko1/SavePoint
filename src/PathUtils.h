#pragma once

#include <filesystem>
#include <system_error>

bool isBackupOutsideSource(const std::filesystem::path &sourcePath, const std::filesystem::path &backupPath, std::error_code &errorCode);