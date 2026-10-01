#include "DirectoryScanner.h"

#include <iostream>
#include <string>

using namespace std;
DirectoryScanner::DirectoryScanner(bool useExclusions) : applyExclusions(useExclusions)
{
}

bool shouldSkipDirectory(const filesystem::path &directoryPath)
{
    string folderName = directoryPath.filename().string();
    return (folderName == ".git" || folderName == "build");
}

ScanSummary DirectoryScanner::scan(const filesystem::path &sourcePath, error_code &errorCode) const
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
            if (applyExclusions && shouldSkipDirectory(current->path()))
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
                cout << "Nie mozna odczytac rozmiaru pliku: " << errorCode.message() << "\n";
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
            cout << "Blad podczas przegladania folderu: " << errorCode.message() << "\n";
            return summary;
        }
    }

    return summary;
}