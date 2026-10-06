#include "PathUtils.h"

using namespace std;

bool isBackupOutsideSource(const filesystem::path &sourcePath, const filesystem::path &backupPath, error_code &errorCode)
{
    filesystem::path checkedPath = backupPath;

    while (true)
    {
        bool sameDirectory = filesystem::equivalent(checkedPath, sourcePath, errorCode);

        if (errorCode)
        {
            return false;
        }

        if (sameDirectory)
        {
            return false;
        }

        filesystem::path parentPath = checkedPath.parent_path();

        if (parentPath == checkedPath)
        {
            return true;
        }

        checkedPath = parentPath;
    }
}