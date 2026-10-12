#include "ScanSummary.h"
#include "DirectoryScanner.h"
#include "FileCopier.h"
#include "BackupInfoWriter.h"
#include "PathUtils.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>
#include <cstdint>
#include <vector>
#include <fstream>

using namespace std;

filesystem::path createBackupVersion(const filesystem::path &backupPath, error_code &errorCode)
{
    int backupNumber = 1;
    filesystem::path backupVersionPath;

    while (true)
    {
        backupVersionPath = backupPath / ("backup_" + to_string(backupNumber));
        bool versionCreated = filesystem::create_directories(backupVersionPath, errorCode);
        if (errorCode)
        {
            return filesystem::path();
        }
        if (versionCreated)
        {
            return backupVersionPath;
        }
        ++backupNumber;
    }
}

int createBackup(const string &folderPath, const string &backupFolderPath)
{

    filesystem::path sourcePath(folderPath);

    error_code errorCode;

    bool isDirectory = filesystem::is_directory(sourcePath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna sprawdzic sciezki: " << errorCode.message() << "\n";
        return 1;
    }

    if (!isDirectory)
    {
        cout << "Podana sciezka nie wskazuje folderu.\n";
        return 1;
    }
    sourcePath = filesystem::canonical(sourcePath, errorCode);
    if (errorCode)
    {
        cout << "Nie mozna ustalic sciezki zrodlowej: " << errorCode.message() << "\n";
        return 1;
    }

    cout << "Folder istnieje: " << folderPath << "\n";

    DirectoryScanner scanner;
    ScanSummary summary = scanner.scan(sourcePath, errorCode);

    if (errorCode)
    {
        return 1;
    }

    cout << "\nPodsumowanie:\n";
    cout << "Liczba folderow: " << summary.directories.size() << "\n";
    cout << "Liczba plikow: " << summary.files.size() << "\n";
    cout << "Laczny rozmiar plikow: " << summary.totalSize << " B\n";

    filesystem::path backupPath(backupFolderPath);
    bool created = filesystem::create_directories(backupPath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna utworzyc folderu kopii zapasowej: " << errorCode.message() << "\n";
        return 1;
    }

    backupPath = filesystem::canonical(backupPath, errorCode);
    if (errorCode)
    {
        cout << "Nie mozna ustalic sciezki kopii zapasowej: " << errorCode.message() << "\n";
        return 1;
    }
    bool backupOutsideSource = isBackupOutsideSource(sourcePath, backupPath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna porownac folderow: " << errorCode.message() << "\n";
        return 1;
    }

    if (!backupOutsideSource)
    {
        cout << "Folder kopii nie moze byc folderem zrodlowym ani jego podfolderem.\n";
        return 1;
    }

    if (created)
    {
        cout << "Folder kopii zapasowej zostal utworzony: " << backupFolderPath << "\n";
    }
    else
    {
        cout << "Folder kopii zapasowej juz istnieje: " << backupFolderPath << "\n";
    }
    filesystem::path backupVersionPath = createBackupVersion(backupPath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna utworzyc folderu wersji kopii zapasowej: " << errorCode.message() << "\n";
        return 1;
    }

    filesystem::path backupDataPath = backupVersionPath / "data";
    filesystem::create_directories(backupDataPath, errorCode);
    if (errorCode)
    {
        cout << "Nie mozna utworzyc folderu danych kopii zapasowej: " << errorCode.message() << "\n";
        return 1;
    }
    FileCopier copier(sourcePath, backupDataPath);

    bool directoriesCreated = copier.createDirectories(summary.directories, errorCode);

    if (!directoriesCreated)
    {
        return 1;
    }

    cout << "\nKopiowanie plikow:\n";

    bool copied = copier.copy(summary.files, errorCode);

    if (!copied)
    {
        return 1;
    }
    BackupInfoWriter infoWriter(backupVersionPath);

    bool infoSaved = infoWriter.write(sourcePath, summary);
    if (!infoSaved)
    {
        return 1;
    }

    cout << "\nKopiowanie plikow zakonczone.\n";

    return 0;
}

int createBackupInteractive()
{
    string folderPath;
    string backupFolderPath;

    cout << "Podaj sciezke do folderu: ";

    if (!getline(cin, folderPath))
    {
        cout << "Nie mozna odczytac sciezki zrodlowej.\n";
        return 1;
    }

    cout << "Podaj sciezke do folderu kopii zapasowej: ";

    if (!getline(cin, backupFolderPath))
    {
        cout << "Nie mozna odczytac sciezki kopii.\n";
        return 1;
    }

    if (folderPath.empty() || backupFolderPath.empty())
    {
        cout << "Sciezki nie moga byc puste.\n";
        return 1;
    }

    return createBackup(folderPath, backupFolderPath);
}

int showBackupInfo()
{
    string versionFolderPath;

    cout << "Podaj sciezke do folderu wersji kopii: ";
    getline(cin, versionFolderPath);

    filesystem::path backupVersionPath(versionFolderPath);
    filesystem::path infoPath = backupVersionPath / "backup_info.txt";

    ifstream infoFile(infoPath);

    if (!infoFile)
    {
        cout << "Nie mozna otworzyc opisu kopii: " << infoPath.string() << "\n";
        return 1;
    }

    cout << "\nOpis kopii:\n";

    string line;

    while (getline(infoFile, line))
    {
        cout << line << "\n";
    }

    if (infoFile.bad() || !infoFile.eof())
    {
        cout << "Blad podczas odczytywania opisu kopii.\n";
        return 1;
    }

    return 0;
}

int listBackups()
{
    string backupFolderPath;

    cout << "Podaj sciezke do glownego folderu kopii: ";

    if (!getline(cin, backupFolderPath))
    {
        cout << "Nie mozna odczytac sciezki.\n";
        return 1;
    }

    filesystem::path backupPath(backupFolderPath);
    error_code errorCode;

    filesystem::directory_iterator current(backupPath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna otworzyc folderu kopii: " << errorCode.message() << "\n";
        return 1;
    }

    filesystem::directory_iterator end;

    cout << "\nFoldery kopii:\n";

    while (current != end)
    {
        filesystem::file_status entryStatus = current->status(errorCode);

        if (errorCode)
        {
            cout << "Nie mozna odczytac informacji o elemencie: " << errorCode.message() << "\n";
            return 1;
        }

        if (filesystem::is_directory(entryStatus))
        {
            string folderName = current->path().filename().string();

            if (folderName.starts_with("backup_"))
            {
                filesystem::path infoPath = current->path() / "backup_info.txt";

                bool infoExists = filesystem::exists(infoPath, errorCode);

                if (errorCode)
                {
                    cout << "Nie mozna sprawdzic obecnosci opisu: " << errorCode.message() << "\n";
                    return 1;
                }

                bool hasInfoFile = false;

                if (infoExists)
                {
                    hasInfoFile = filesystem::is_regular_file(infoPath, errorCode);

                    if (errorCode)
                    {
                        cout << "Nie mozna sprawdzic typu pliku opisu: " << errorCode.message() << "\n";
                        return 1;
                    }
                }

                if (hasInfoFile)
                {
                    cout << "[Z OPISEM] ";
                }
                else
                {
                    cout << "[BRAK PLIKU OPISU] ";
                }

                cout << folderName << "\n";
            }
        }

        current.increment(errorCode);

        if (errorCode)
        {
            cout << "Blad podczas przegladania folderu: " << errorCode.message() << "\n";
            return 1;
        }
    }

    return 0;
}

int restoreBackup()
{
    string versionFolderPath;

    cout << "Podaj sciezke do folderu wersji kopii: ";

    if (!getline(cin, versionFolderPath))
    {
        cout << "Nie mozna odczytac sciezki.\n";
        return 1;
    }

    if (versionFolderPath.empty())
    {
        cout << "Sciezka nie moze byc pusta.\n";
        return 1;
    }

    filesystem::path backupVersionPath(versionFolderPath);
    filesystem::path backupDataPath = backupVersionPath / "data";

    error_code errorCode;

    bool isDataDirectory = filesystem::is_directory(backupDataPath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna sprawdzic folderu danych kopii: " << errorCode.message() << "\n";
        return 1;
    }

    if (!isDataDirectory)
    {
        cout << "Sciezka data nie wskazuje folderu.\n";
        return 1;
    }

    cout << "\nFolder danych do przywrocenia: " << backupDataPath.string() << "\n";
    string restoreFolderPath;

    cout << "Podaj sciezke do folderu docelowego: ";

    if (!getline(cin, restoreFolderPath))
    {
        cout << "Nie mozna odczytac sciezki docelowej.\n";
        return 1;
    }

    if (restoreFolderPath.empty())
    {
        cout << "Sciezka docelowa nie moze byc pusta.\n";
        return 1;
    }

    filesystem::path restorePath(restoreFolderPath);

    filesystem::create_directories(restorePath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna przygotowac folderu docelowego: " << errorCode.message() << "\n";
        return 1;
    }

    bool isEmpty = filesystem::is_empty(restorePath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna sprawdzic zawartosci folderu docelowego: " << errorCode.message() << "\n";
        return 1;
    }

    if (!isEmpty)
    {
        cout << "Folder docelowy musi byc pusty.\n";
        return 1;
    }
    backupVersionPath = filesystem::canonical(backupVersionPath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna ustalic sciezki wersji kopii: " << errorCode.message() << "\n";
        return 1;
    }

    restorePath = filesystem::canonical(restorePath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna ustalic sciezki docelowej: " << errorCode.message() << "\n";
        return 1;
    }

    bool restoreOutsideBackup = isBackupOutsideSource(backupVersionPath, restorePath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna porownac folderow: " << errorCode.message() << "\n";
        return 1;
    }

    if (!restoreOutsideBackup)
    {
        cout << "Folder docelowy nie moze byc folderem wersji kopii ani jego podfolderem.\n";
        return 1;
    }

    cout << "Wybrano pusty folder docelowy: " << restorePath.string() << "\n";
    DirectoryScanner scanner(false);
    ScanSummary restoreSummary = scanner.scan(backupDataPath, errorCode);
    if (errorCode)
    {
        return 1;
    }

    cout << "\nDo przywrocenia:\n";
    cout << "Liczba folderow: " << restoreSummary.directories.size() << "\n";

    cout << "Liczba plikow: " << restoreSummary.files.size() << "\n";

    cout << "Laczny rozmiar plikow: " << restoreSummary.totalSize << " B\n";
    FileCopier copier(backupDataPath, restorePath);

    bool directoriesCreated = copier.createDirectories(restoreSummary.directories, errorCode);

    if (!directoriesCreated)
    {
        return 1;
    }

    cout << "\nPrzywracanie plikoww:\n";

    bool copied = copier.copy(restoreSummary.files, errorCode);

    if (!copied)
    {
        return 1;
    }

    cout << "\nPrzywracanie zakonczone.\n";
    cout << "Folder docelowy: " << restorePath.string() << "\n";
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc == 2 && string(argv[1]) == "--help")
    {
        cout << "SavePoint - kopie zapasowe folderow\n\n";
        cout << "Dostepne argumenty:\n";
        cout << "  backup  Utworz kopie zapasowa - program zapyta o sciezki\n";
        cout << "  --help  Wyswietl pomoc\n\n";
        cout << "Bez argumentow program uruchamia menu.\n";

        return 0;
    }
    if (argc == 2 && string(argv[1]) == "backup")
    {
        return createBackupInteractive();
    }

    if (argc > 1)
    {
        cout << "Nieprawidlowe argumenty. Uzyj --help, aby zobaczyc pomoc.\n";
        return 1;
    }
    cout << "SavePoint uruchomiony!\n";

    while (true)
    {
        cout << "\n1. Utworz kopie zapasowa\n";
        cout << "2. Wyswietl opis kopii\n";
        cout << "3. Wyswietl liste kopii\n";
        cout << "4. Przywroc kopie zapasowa\n";
        cout << "0. Zakoncz program\n";
        cout << "Wybor: ";

        string choice;

        if (!getline(cin, choice))
        {
            cout << "Nie mozna odczytac wyboru.\n";
            return 1;
        }

        if (choice == "0")
        {
            return 0;
        }

        int result = 0;

        if (choice == "1")
        {
            result = createBackupInteractive();
        }
        else if (choice == "2")
        {
            result = showBackupInfo();
        }
        else if (choice == "3")
        {
            result = listBackups();
        }
        else if (choice == "4")
        {
            result = restoreBackup();
        }
        else
        {
            cout << "Nieprawidlowy wybor.\n";
            continue;
        }

        if (result != 0)
        {
            cout << "Operacja zakonczyla sie bledem.\n";
        }
    }
}