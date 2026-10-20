#include "PathUtils.h"

using namespace std;

bool isBackupOutsideSource(const filesystem::path &sourcePath, const filesystem::path &backupPath, error_code &errorCode)
{
    errorCode.clear();

    filesystem::path checkedPath =
        filesystem::absolute(backupPath, errorCode);

    if (errorCode)
    {
        return false;
    }

    checkedPath = filesystem::weakly_canonical(checkedPath, errorCode);

    if (errorCode)
    {
        return false;
    }

    while (true)
    {
        bool pathExists = filesystem::exists(checkedPath, errorCode);

        if (errorCode)
        {
            return false;
        }

        if (pathExists)
        {
            bool sameDirectory =
                filesystem::equivalent(checkedPath, sourcePath, errorCode);

            if (errorCode)
            {
                return false;
            }

            if (sameDirectory)
            {
                return false;
            }
        }

        filesystem::path parentPath = checkedPath.parent_path();

        if (parentPath == checkedPath)
        {
            return true;
        }

        checkedPath = parentPath;
    }
}