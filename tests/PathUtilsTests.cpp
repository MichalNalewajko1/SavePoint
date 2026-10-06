#include "PathUtils.h"

#include <filesystem>
#include <iostream>
#include <system_error>

using namespace std;

int main()
{
    error_code errorCode;

    filesystem::path sourcePath = filesystem::current_path(errorCode);

    if (errorCode)
    {
        cout << "Nie mozna ustalic biezacego folderu: "<< errorCode.message() << "\n";
        return 1;
    }

    bool backupOutsideSource = isBackupOutsideSource(sourcePath, sourcePath, errorCode);

    if (errorCode)
    {
        cout << "Blad podczas sprawdzania folderow: "<< errorCode.message() << "\n";
        return 1;
    }

    if (backupOutsideSource)
    {
        cout << "TEST NIEUDANY: zaakceptowano folder zrodlowy jako folder kopii.\n";
        return 1;
    }

    cout << "TEST OK: folder zrodlowy zostal odrzucony jako folder kopii.\n";

    return 0;
}