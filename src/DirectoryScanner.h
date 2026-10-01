#pragma once

#include "ScanSummary.h"

#include <filesystem>
#include <system_error>

ScanSummary scanDirectory(const std::filesystem::path &sourcePath, std::error_code &errorCode, bool applyExclusions = true);