#include <iostream>
#include <string>
#include <filesystem>
#include <system_error>
#include <cstdint>

using namespace std;

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

    filesystem::recursive_directory_iterator current(sourcePath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna otworzyc folderu: "
             << errorCode.message() << "\n";
        return 1;
    }

    filesystem::recursive_directory_iterator end;

    cout << "Zawartosc folderu:\n";

    uintmax_t fileCount = 0;
    uintmax_t totalSize = 0;

    while (current != end)
    {
        filesystem::file_status entryStatus = current->status(errorCode);

        if (errorCode)
        {
            cout << "Nie mozna odczytac informacji o elemencie: "
                 << errorCode.message() << "\n";
            return 1;
        }

        if (filesystem::is_directory(entryStatus))
        {
            string folderName = current->path().filename().string();
            if (folderName == ".git" || folderName == "build")
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
                return 1;
            }

            ++fileCount;
            totalSize += fileSize;

            cout << "[PLIK, " << fileSize << " B] ";
        }
        else
        {
            cout << "[INNY] ";
        }

        filesystem::path relativePath = current->path().lexically_relative(sourcePath);

        cout << relativePath.string() << "\n";
        current.increment(errorCode);

        if (errorCode)
        {
            cout << "Blad podczas przegladania folderu: "
                 << errorCode.message() << "\n";
            return 1;
        }
    }

    cout << "\nPodsumowanie:\n";
    cout << "Liczba plikow: " << fileCount << "\n";
    cout << "Laczny rozmiar plikow: " << totalSize << " B\n";

    return 0;
}