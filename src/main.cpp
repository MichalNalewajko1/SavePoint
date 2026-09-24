#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>
#include <cstdint>
#include <vector>
#include <fstream>

using namespace std;

struct ScanSummary
{
    vector<filesystem::path> files;
    vector<filesystem::path> directories;
    uintmax_t totalSize = 0;
};

bool shouldSkipDirectory(const filesystem::path &directoryPath)
{
    string folderName = directoryPath.filename().string();
    return (folderName == ".git" || folderName == "build");
}

ScanSummary scanDirectory(const filesystem::path &sourcePath, error_code &errorCode)
{
    ScanSummary summary;

    filesystem::recursive_directory_iterator current(sourcePath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna otworzyc folderu: "
             << errorCode.message() << "\n";
        return summary;
    }

    filesystem::recursive_directory_iterator end;

    cout << "Zawartosc folderu:\n";
    while (current != end)
    {
        filesystem::file_status entryStatus = current->status(errorCode);

        if (errorCode)
        {
            cout << "Nie mozna odczytac informacji o elemencie: "
                 << errorCode.message() << "\n";
            return summary;
        }
        filesystem::path relativePath = current->path().lexically_relative(sourcePath);

        if (filesystem::is_directory(entryStatus))
        {
            if (shouldSkipDirectory(current->path()))
            {
                current.disable_recursion_pending();
                cout << "[POMINIETY FOLDER] ";
            }
            else
            {
                summary.directories.push_back(relativePath);
                cout << "[FOLDER] ";
            }
        }
        else if (filesystem::is_regular_file(entryStatus))
        {
            uintmax_t fileSize = current->file_size(errorCode);

            if (errorCode)
            {
                cout << "Nie mozna odczytac rozmiaru pliku: "
                     << errorCode.message() << "\n";
                return summary;
            }

            summary.files.push_back(relativePath);
            summary.totalSize += fileSize;

            cout << "[PLIK, " << fileSize << " B] ";
        }
        else
        {
            cout << "[INNY] ";
        }

        cout << relativePath.string() << "\n";
        current.increment(errorCode);

        if (errorCode)
        {
            cout << "Blad podczas przegladania folderu: "
                 << errorCode.message() << "\n";
            return summary;
        }
    }

    return summary;
}

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
bool copyFiles(const filesystem::path &sourcePath, const filesystem::path &backupVersionPath, const vector<filesystem::path> &files, error_code &errorCode)
{
    errorCode.clear();

    for (const filesystem::path &filePath : files)
    {
        filesystem::path sourceFilePath = sourcePath / filePath;
        filesystem::path destinationFilePath = backupVersionPath / filePath;
        filesystem::path destinationDirectory = destinationFilePath.parent_path();

        filesystem::create_directories(destinationDirectory, errorCode);

        if (errorCode)
        {
            cout << "Nie mozna utworzyc podfolderu kopii: " << errorCode.message() << "\n";
            return false;
        }

        filesystem::copy_file(sourceFilePath, destinationFilePath, errorCode);

        if (errorCode)
        {
            cout << "Nie mozna skopiowac pliku: " << filePath.string() << " - " << errorCode.message() << "\n";
            return false;
        }

        cout << "[SKOPIOWANO] " << filePath.string() << "\n";
        cout << "Zrodlo: " << sourceFilePath.string() << "\n";
        cout << "Cel: " << destinationFilePath.string() << "\n";
    }

    return true;
}
bool createBackupDirectories(const filesystem::path &backupVersionPath, const vector<filesystem::path> &directories, error_code &errorCode)
{
    errorCode.clear();

    for (const filesystem::path &directoryPath : directories)
    {
        filesystem::path destinationDirectory = backupVersionPath / directoryPath;

        filesystem::create_directories(destinationDirectory, errorCode);

        if (errorCode)
        {
            cout << "Nie mozna utworzyc folderu w kopii: "
                 << errorCode.message() << "\n";
            return false;
        }
    }

    return true;
}

bool writeBackupInfo(const filesystem::path &sourcePath, const filesystem::path &backupVersionPath, const ScanSummary &summary)
{
    filesystem::path infoPath = backupVersionPath / "backup_info.tmp";
    ofstream infoFile(infoPath);

    if (!infoFile)
    {
        cout << "Nie mozna otworzyc pliku opisu kopii: " << infoPath.string() << "\n";
        return false;
    }

    infoFile << "SavePoint - opis kopii\n";
    infoFile << "Zrodlo: " << sourcePath.string() << "\n";
    infoFile << "Liczba folderow: " << summary.directories.size() << "\n";
    infoFile << "Liczba plikow: " << summary.files.size() << "\n";
    infoFile << "Laczny rozmiar plikow: " << summary.totalSize << " B\n";
    infoFile << "Lista folderow:\n";
    for (const filesystem::path &directoryPath : summary.directories)
    {
        infoFile << directoryPath.string() << "\n";
    }
    infoFile << "Lista plikow:\n";
    for (const filesystem::path &filePath : summary.files)
    {
        infoFile << filePath.string() << "\n";
    }

    infoFile.close();

    if (!infoFile)
    {
        cout << "Nie udalo sie zapisac opisu kopii: "
             << infoPath.string() << "\n";
        return false;
    }

    filesystem::path finalInfoPath = backupVersionPath / "backup_info.txt";

    error_code errorCode;

    filesystem::rename(infoPath, finalInfoPath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna przeniesc pliku opisu kopii: " << errorCode.message() << "\n";
        return false;
    }
    return true;
}

int createBackup()
{
    string folderPath;

    cout << "Podaj sciezke do folderu: ";
    getline(cin, folderPath);

    filesystem::path sourcePath(folderPath);

    error_code errorCode;

    bool isDirectory = filesystem::is_directory(sourcePath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna sprawdzic sciezki: "
             << errorCode.message() << "\n";
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
        cout << "Nie mozna ustalic sciezki zrodlowej: "
             << errorCode.message() << "\n";
        return 1;
    }

    cout << "Folder istnieje: " << folderPath << "\n";

    ScanSummary summary = scanDirectory(sourcePath, errorCode);

    if (errorCode)
    {
        return 1;
    }

    cout << "\nPodsumowanie:\n";
    cout << "Liczba folderow: " << summary.directories.size() << "\n";
    cout << "Liczba plikow: " << summary.files.size() << "\n";
    cout << "Laczny rozmiar plikow: " << summary.totalSize << " B\n";
    string backupFolderPath;

    cout << "Podaj sciezke do folderu kopii zapasowej: ";
    getline(cin, backupFolderPath);

    filesystem::path backupPath(backupFolderPath);
    bool created = filesystem::create_directories(backupPath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna utworzyc folderu kopii zapasowej: "
             << errorCode.message() << "\n";
        return 1;
    }

    backupPath = filesystem::canonical(backupPath, errorCode);
    if (errorCode)
    {
        cout << "Nie mozna ustalic sciezki kopii zapasowej: "
             << errorCode.message() << "\n";
        return 1;
    }
    bool backupOutsideSource = isBackupOutsideSource(sourcePath, backupPath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna porownac folderow: "
             << errorCode.message() << "\n";
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
    bool directoriesCreated = createBackupDirectories(backupDataPath, summary.directories, errorCode);

    if (!directoriesCreated)
    {
        return 1;
    }

    cout << "\nKopiowanie plikow:\n";

    bool copied = copyFiles(sourcePath, backupDataPath, summary.files, errorCode);

    if (!copied)
    {
        return 1;
    }
    bool infoSaved = writeBackupInfo(sourcePath, backupVersionPath, summary);

    if (!infoSaved)
    {
        return 1;
    }

    cout << "\nKopiowanie plikow zakonczone.\n";

    return 0;
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
        cout << "Nie mozna otworzyc opisu kopii: "<< infoPath.string() << "\n";
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

int main()
{
    cout << "SavePoint uruchomiony!\n";

    while (true)
    {
        cout << "\n1. Utworz kopie zapasowa\n";
        cout << "2. Wyswietl opis kopii\n";
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
            result = createBackup();
        }
        else if (choice == "2")
        {
            result = showBackupInfo();
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