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

    filesystem::directory_iterator current(sourcePath, errorCode);

    if (errorCode)
    {
        cout << "Nie mozna otworzyc folderu: "
             << errorCode.message() << "\n";
        return 1;
    }

    filesystem::directory_iterator end;

    cout << "Zawartosc folderu:\n";

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
            cout << "[FOLDER] ";
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

            cout << "[PLIK, " << fileSize << " B] ";
        }
        else
        {
            cout << "[INNY] ";
        }

        cout << current->path().filename().string() << "\n";

        current.increment(errorCode);

        if (errorCode)
        {
            cout << "Blad podczas przegladania folderu: "
                 << errorCode.message() << "\n";
            return 1;
        }
    }

    return 0;
}