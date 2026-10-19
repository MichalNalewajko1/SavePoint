# SavePoint

A console application written in C++20 for creating and restoring
local folder backups.

I am developing this project as part of learning C++, working with
the file system, object-oriented programming, and software testing.

## Features

- Creating successive backup versions: backup_1, backup_2, etc.
- Preserving the subfolder structure, including empty folders.
- Skipping `.git` and `build` folders when creating a backup.
- Saving a backup description with a list of files, folders, and total size.
- Displaying available backup versions and their descriptions.
- Restoring data to an empty folder.
- Rejecting a backup destination located inside the source
  or being the same folder.

## Building on Windows with MinGW

Required tools:

- a g++ compiler supporting C++20;
- mingw32-make;
- CMake.

Run the commands in the project root folder:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

Running the application:

```powershell
.\build\SavePoint.exe
```

The menu allows you to create a backup, display its description,
display the list of backups, or restore data.

## Running tests

After building the project, run:

```powershell
ctest --test-dir build --output-on-failure
```

The `PathUtilsTests` program checks the location of the backup folder
relative to the source folder:

- rejecting the same folder;
- accepting the parent folder;
- rejecting a subfolder of the source.

## Automated checks

GitHub Actions builds the project and runs tests on Windows
when changes are pushed to the `main` branch and for pull requests
targeting this branch.

The configuration is located in `.github/workflows/build.yml`.

## Code organization

- `DirectoryScanner` — scanning folders and collecting information.
- `ScanSummary` — scan result: files, folders, and total size.
- `FileCopier` — creating destination folders and copying files.
- `BackupInfoWriter` — saving the backup description.
- `PathUtils` — checking folder locations.
- `main.cpp` — menu and handling user-selected operations.