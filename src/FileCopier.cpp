#include "FileCopier.h"

#include <iostream>
#include <cstddef>

using namespace std;

FileCopier::FileCopier(const filesystem::path &source, const filesystem::path &destination) : sourcePath(source), destinationPath(destination)
{
}

bool FileCopier::copy(const vector<filesystem::path> &files, error_code &errorCode) const
{
    errorCode.clear();
    std::size_t copiedFiles = 0;

    for (const filesystem::path &filePath : files)
    {
        filesystem::path sourceFilePath = sourcePath / filePath;
        filesystem::path destinationFilePath = destinationPath / filePath;
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

        ++copiedFiles;

        std::cout << "[SKOPIOWANO "<< copiedFiles << "/" << files.size()<< "] " << filePath.string() << "\n";
        cout << "Zrodlo: " << sourceFilePath.string() << "\n";
        cout << "Cel: " << destinationFilePath.string() << "\n";
    }

    return true;
}

bool FileCopier::createDirectories(const vector<filesystem::path> &directories, error_code &errorCode) const
{
    errorCode.clear();

    for (const filesystem::path &directoryPath : directories)
    {
        filesystem::path destinationDirectory = destinationPath / directoryPath;

        filesystem::create_directories(destinationDirectory, errorCode);

        if (errorCode)
        {
            cout << "Nie mozna utworzyc folderu docelowego: " << errorCode.message() << "\n";
            return false;
        }
    }

    return true;
}