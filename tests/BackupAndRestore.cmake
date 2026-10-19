cmake_minimum_required(VERSION 3.16)

set(testRoot "${CMAKE_CURRENT_BINARY_DIR}/BackupAndRestore")
set(sourcePath "${testRoot}/source")
set(backupPath "${testRoot}/backups")
set(restorePath "${testRoot}/restored")
set(expectedContent "Pierwsza wersja pliku\n")

file(REMOVE_RECURSE "${testRoot}")

file(MAKE_DIRECTORY "${sourcePath}/docs")
file(MAKE_DIRECTORY "${sourcePath}/empty")
file(WRITE "${sourcePath}/docs/test.txt" "${expectedContent}")

execute_process(
    COMMAND "${SAVEPOINT_EXE}" backup "${sourcePath}" "${backupPath}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
    TIMEOUT 5
)

if(NOT "${result}" STREQUAL "0")
    message(FATAL_ERROR
        "Tworzenie kopii nie powiodlo sie: ${result}\n${output}\n${errors}"
    )
endif()

file(WRITE "${sourcePath}/docs/test.txt" "Druga wersja pliku\n")

set(backupVersionPath "${backupPath}/backup_1")

execute_process(
    COMMAND "${SAVEPOINT_EXE}" restore "${backupVersionPath}" "${restorePath}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
    TIMEOUT 5
)

if(NOT "${result}" STREQUAL "0")
    message(FATAL_ERROR
        "Przywracanie nie powiodlo sie: ${result}\n${output}\n${errors}"
    )
endif()

set(restoredFilePath "${restorePath}/docs/test.txt")

if(NOT EXISTS "${restoredFilePath}")
    message(FATAL_ERROR "Brakuje przywroconego pliku docs/test.txt.")
endif()

file(READ "${restoredFilePath}" actualContent)

if(NOT "${actualContent}" STREQUAL "${expectedContent}")
    message(FATAL_ERROR
        "Przywrocony plik nie zawiera tresci zapisanej w kopii."
    )
endif()

if(NOT IS_DIRECTORY "${restorePath}/empty")
    message(FATAL_ERROR "Nie przywrocono pustego folderu empty.")
endif()

file(REMOVE_RECURSE "${testRoot}")

message(STATUS "TEST OK: utworzono kopie i przywrocono zapisane dane.")