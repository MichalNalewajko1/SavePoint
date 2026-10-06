#include "BackupInfoWriter.h"

#include <fstream>
#include <iostream>
#include <system_error>

using namespace std;

BackupInfoWriter::BackupInfoWriter(const filesystem::path &versionPath) : backupVersionPath(versionPath)
{
}

bool BackupInfoWriter::write(const filesystem::path &sourcePath, const ScanSummary &summary) const
{
    filesystem::path infoPath = backupVersionPath / "backup_info.tmp";

    ofstream infoFile(infoPath);

    if (!infoFile)
    {
        cout << "Nie mozna otworzyc pliku opisu kopii: "<< infoPath.string() << "\n";
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
        cout << "Nie udalo sie zapisac opisu kopii: "<< infoPath.string() << "\n";
        return false;
    }

    filesystem::path finalInfoPath = backupVersionPath / "backup_info.txt";

    error_code errorCode;

    filesystem::rename(infoPath, finalInfoPath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna przeniesc pliku opisu kopii: "<< errorCode.message() << "\n";
        return false;
    }

    return true;
}