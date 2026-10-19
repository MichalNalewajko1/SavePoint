# SavePoint

Aplikacja konsolowa napisana w C++20 do tworzenia i przywracania
lokalnych kopii zapasowych folderów.

Projekt rozwijam w ramach nauki C++, pracy z systemem plików,
programowania obiektowego i testowania oprogramowania.

## Funkcje

- Tworzenie kolejnych wersji kopii: backup_1, backup_2 itd.
- Zachowywanie struktury podfolderów, w tym pustych folderów.
- Pomijanie folderów `.git` i `build` podczas tworzenia kopii.
- Zapisywanie opisu kopii z listą plików, folderów i łącznym rozmiarem.
- Wyświetlanie dostępnych wersji kopii i ich opisów.
- Przywracanie danych do pustego folderu.
- Odrzucanie celu kopii znajdującego się wewnątrz źródła
  lub będącego tym samym folderem.

## Budowanie na Windows z MinGW

Wymagane narzędzia:

- kompilator g++ obsługujący C++20;
- mingw32-make;
- CMake.

Polecenia wykonuj w głównym folderze projektu:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

Uruchomienie aplikacji:

```powershell
.\build\SavePoint.exe
```

W menu można wybrać tworzenie kopii, wyświetlenie opisu,
wyświetlenie listy kopii lub przywracanie danych.

## Uruchamianie testów

Po zbudowaniu projektu wykonaj:

```powershell
ctest --test-dir build --output-on-failure
```

Program `PathUtilsTests` sprawdza położenie folderu kopii
względem folderu źródłowego:

- odrzucenie tego samego folderu;
- zaakceptowanie folderu nadrzędnego;
- odrzucenie podfolderu źródła.

## Automatyczne sprawdzanie

GitHub Actions buduje projekt i uruchamia testy na Windows
po wysłaniu zmian do gałęzi `main` oraz przy pull requestach
kierowanych do tej gałęzi.

Konfiguracja znajduje się w `.github/workflows/build.yml`.

## Organizacja kodu

- `DirectoryScanner` — przeglądanie folderów i zbieranie informacji.
- `ScanSummary` — wynik skanowania: pliki, foldery i łączny rozmiar.
- `FileCopier` — tworzenie folderów docelowych i kopiowanie plików.
- `BackupInfoWriter` — zapisywanie opisu kopii.
- `PathUtils` — sprawdzanie położenia folderów.
- `main.cpp` — menu i obsługa operacji wybranych przez użytkownika.

## Architektura

Diagram komponentów, przepływy backupu i restore oraz układ danych kopii
znajdują się w pliku [ARCHITECTURE.md](ARCHITECTURE.md).