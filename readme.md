# SavePoint

A C++20 console application for creating and restoring local folder backups. It supports an interactive menu and command-line commands.

This project was developed to practise modern C++, filesystem operations, object-oriented design, error handling, and automated testing.

The console interface and backup reports are currently in Polish.

## Features

- Creates separate backup versions: `backup_1`, `backup_2`, and so on.
- Preserves file contents and directory structure, including empty directories.
- Excludes directories named `.git` and `build` during backup scans.
- Displays copying progress as the number of completed files.
- Saves a backup report containing the local timestamp, source path, file and directory counts, total file size, and relative paths.
- Lists backup versions in numerical order and indicates whether a report exists.
- Restores files into a new or existing empty directory.
- Validates destination locations before creating destination directories.
- Rejects symbolic links encountered during scanning.

## Requirements

- CMake 3.20 or newer.
- A C++ compiler with C++20 support.
- A build tool compatible with the selected CMake generator.

The Windows instructions below use MinGW-w64. Both `g++` and `mingw32-make` must be available in `PATH`.

The application uses the C++ standard library and does not require external C++ libraries.

## Building on Windows

Run these commands from the project root in PowerShell:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

The executable will be available at:

```text
build/SavePoint.exe
```

## Interactive Mode

Start the application without arguments:

```powershell
.\build\SavePoint.exe
```

The menu provides four operations:

1. Create a backup.
2. Display a backup report.
3. List backup versions.
4. Restore a backup.

Choose `0` to exit. After an operation, the application returns to the menu.

## Command-Line Usage

Replace the example paths with your own locations.

### Display help

```powershell
.\build\SavePoint.exe --help
```

### Create a backup

```powershell
.\build\SavePoint.exe backup "C:\Projects\MyProject" "C:\Backups\MyProject"
```

The source directory must already exist. The backup destination is created if necessary.

You can also start the backup operation and enter both paths when prompted:

```powershell
.\build\SavePoint.exe backup
```

### List backup versions

```powershell
.\build\SavePoint.exe list "C:\Backups\MyProject"
```

The list includes directories named `backup_` followed by a positive integer without leading zeros.

Each entry indicates whether `backup_info.txt` exists as a regular file.

### Display a backup report

```powershell
.\build\SavePoint.exe info "C:\Backups\MyProject\backup_1"
```

### Restore a backup

```powershell
.\build\SavePoint.exe restore "C:\Backups\MyProject\backup_1" "C:\Restored\MyProject"
```

Pass the backup version directory, such as `backup_1`, rather than its `data` subdirectory.

The destination is created if it does not exist. An existing destination must be empty.

Command-line operations return `0` on success and `1` on an error.

## Backup Layout

Each backup is stored in a separate version directory.

For example, `C:\Backups\MyProject\backup_1` contains:

| Path | Contents |
| --- | --- |
| `data/` | Copied files and directories, preserving their relative paths. |
| `backup_info.txt` | Backup timestamp, source path, summary, and lists of files and directories. |

After copying succeeds, the report is written to `backup_info.tmp`. The program checks the stream after closing it and then renames the temporary file to `backup_info.txt`.

Restoring requires both a `data` directory and a regular `backup_info.txt` file. Older backup versions without a report are rejected.

## Destination Checks

- The backup destination cannot be the source directory or a directory inside it.
- The restore destination cannot be the selected backup version directory or a directory inside it.
- The restore destination must be empty.
- Location checks support destination paths that do not yet exist.
- Rejected destination locations are checked before the program creates those directories.

These checks help prevent copying a backup into itself and overwriting existing files during restoration.

## Running Tests

After building the project, run:

```powershell
ctest --test-dir build --output-on-failure
```

To display output from successful tests as well:

```powershell
ctest --test-dir build --verbose
```

The project currently registers five CTest tests:

| Test | What it checks |
| --- | --- |
| `PathUtilsTests` | Rejects the source itself and its subdirectories as backup destinations; accepts the source's parent directory. |
| `RestoreMissingDestination` | Rejects a restore command without a destination argument. |
| `RestoreMissingInfo` | Rejects a backup without `backup_info.txt` and does not create the restore destination. |
| `BackupAndRestore` | Creates a backup, changes the original file, and verifies that restoration recovers the saved contents and an empty directory. |
| `RestoreNonEmptyDestination` | Rejects a nonempty restore destination, preserves its existing file, and does not copy additional files into it. |

The integration tests use temporary test directories inside the build directory.

## Continuous Integration

A GitHub Actions workflow is configured to build the project and run CTest on Windows:

- On pushes to `main`.
- On pull requests targeting `main`.

The workflow uses the Debug configuration and is defined in:

```text
.github/workflows/build.yml
```

## Code Organization

| Component | Responsibility |
| --- | --- |
| `DirectoryScanner` | Traverses directories, applies backup exclusions, rejects symbolic links, and collects scan results. |
| `ScanSummary` | Stores relative file paths, directory paths, and total file size. |
| `FileCopier` | Creates destination directories and copies files while reporting progress. |
| `BackupInfoWriter` | Writes and finalizes the backup report. |
| `PathUtils` | Validates the destination's location relative to the source. |
| `main.cpp` | Handles the menu, command-line arguments, and backup and restore operations. |
| `tests/` | Contains C++ tests and CMake integration test scripts. |

## Current Limitations

- Each backup is a full copy. Incremental backups, compression, and encryption are not implemented.
- Symbolic links encountered during scanning cause the operation to fail. Other special file types are skipped.
- The program does not calculate checksums or verify copied file contents. The presence of a backup report alone does not prove data integrity.
- Files should not be modified while a backup is running. The application does not create a filesystem snapshot.
- Failed operations may leave partially created backup or restore directories. Automatic rollback and cleanup are not implemented.