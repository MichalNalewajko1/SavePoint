#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>
#include <cstdint>
#include <vector>

using namespace std;

struct ScanSummary
{
    vector<filesystem::path> files;
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

int main()
{
    cout << "SavePoint uruchomiony!\n";

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

    cout << "Folder istnieje: " << folderPath << "\n";

    ScanSummary summary = scanDirectory(sourcePath, errorCode);

    if (errorCode)
    {
        return 1;
    }

    cout << "\nPodsumowanie:\n";
    cout << "Liczba plikow: " << summary.files.size() << "\n";
    cout << "Laczny rozmiar plikow: " << summary.totalSize << " B\n";
    string backupFolderPath;
    
    cout << "Podaj sciezke do folderu kopii zapasowej: ";
    getline(cin, backupFolderPath);

    filesystem::path backupPath(backupFolderPath);

    cout<<"\nPlan kopiowania:\n";
    for (const filesystem::path &filePath : summary.files)
    {
        filesystem::path sourceFilePath = sourcePath / filePath;
        filesystem::path destinationFilePath = backupPath / filePath;
        cout<<"Zrodlo: "<<sourceFilePath.string()<<"\n";
        cout<<"Cel: "<<destinationFilePath.string()<<"\n";
    }

    return 0;
}