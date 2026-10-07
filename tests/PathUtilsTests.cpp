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
        cout << "Nie mozna ustalic biezacego folderu: " << errorCode.message() << "\n";
        return 1;
    }

    bool backupOutsideSource = isBackupOutsideSource(sourcePath, sourcePath, errorCode);

    if (errorCode)
    {
        cout << "Blad podczas sprawdzania folderow: " << errorCode.message() << "\n";
        return 1;
    }

    if (backupOutsideSource)
    {
        cout << "TEST NIEUDANY: zaakceptowano folder zrodlowy jako folder kopii.\n";
        return 1;
    }

    cout << "TEST OK: folder zrodlowy zostal odrzucony jako folder kopii.\n";

    filesystem::path backupPath = sourcePath.parent_path();

    if (backupPath == sourcePath)
    {
        cout << "Nie mozna przygotowac testu: biezacy folder nie ma innego folderu nadrzednego.\n";
        return 1;
    }

    bool parentOutsideSource = isBackupOutsideSource(sourcePath, backupPath, errorCode);

    if (errorCode)
    {
        cout << "Blad podczas sprawdzania folderu nadrzednego: " << errorCode.message() << "\n";
        return 1;
    }

    if (!parentOutsideSource)
    {
        cout << "TEST NIEUDANY: odrzucono folder nadrzedny wobec zrodla.\n";
        return 1;
    }

    cout << "TEST OK: zaakceptowano folder nadrzedny wobec zrodla.\n";

    filesystem::path testSourcePath = sourcePath.parent_path();
    filesystem::path testBackupPath = sourcePath;

    bool childOutsideSource = isBackupOutsideSource(testSourcePath, testBackupPath, errorCode);

    if (errorCode)
    {
        cout << "Blad podczas sprawdzania podfolderu: "<< errorCode.message() << "\n";
        return 1;
    }

    if (childOutsideSource)
    {
        cout << "TEST NIEUDANY: zaakceptowano podfolder zrodla jako cel kopii.\n";
        return 1;
    }

    cout << "TEST OK: podfolder zrodla zostal odrzucony jako cel kopii.\n";

    return 0;
}