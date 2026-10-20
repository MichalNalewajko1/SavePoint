cmake_minimum_required(VERSION 3.16)

set(testRoot "${CMAKE_CURRENT_BINARY_DIR}/RestoreNonEmptyDestination")
set(sourcePath "${testRoot}/source")
set(backupPath "${testRoot}/backups")
set(restorePath "${testRoot}/restored")
set(expectedContent "Wazne dane uzytkownika\n")

file(REMOVE_RECURSE "${testRoot}")

file(MAKE_DIRECTORY "${sourcePath}")
file(MAKE_DIRECTORY "${restorePath}")

file(WRITE "${sourcePath}/test.txt" "Tresc zapisana w kopii\n")
file(WRITE "${sourcePath}/new.txt" "Dodatkowy plik z kopii\n")
file(WRITE "${restorePath}/test.txt" "${expectedContent}")

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

set(backupVersionPath "${backupPath}/backup_1")

execute_process(
    COMMAND "${SAVEPOINT_EXE}" restore "${backupVersionPath}" "${restorePath}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
    TIMEOUT 5
)

if(NOT "${result}" STREQUAL "1")
    message(FATAL_ERROR
        "Oczekiwano odmowy z kodem 1, otrzymano: ${result}\n${output}\n${errors}"
    )
endif()

string(FIND "${output}" "Folder docelowy musi byc pusty." messagePosition)

if(messagePosition EQUAL -1)
    message(FATAL_ERROR
        "Brakuje komunikatu o niepustym folderze docelowym.\n${output}\n${errors}"
    )
endif()

if(NOT EXISTS "${restorePath}/test.txt")
    message(FATAL_ERROR "Istniejacy plik test.txt zostal usuniety.")
endif()

file(READ "${restorePath}/test.txt" actualContent)

if(NOT "${actualContent}" STREQUAL "${expectedContent}")
    message(FATAL_ERROR "Tresc istniejacego pliku zostala zmieniona.")
endif()

if(EXISTS "${restorePath}/new.txt")
    message(FATAL_ERROR
        "Program skopiowal dodatkowy plik do niepustego folderu."
    )
endif()

file(REMOVE_RECURSE "${testRoot}")

message(STATUS "TEST OK: odrzucono przywracanie i zachowano istniejace dane.")