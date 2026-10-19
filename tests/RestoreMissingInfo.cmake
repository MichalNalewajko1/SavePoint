cmake_minimum_required(VERSION 3.16)

set(testRoot "${CMAKE_CURRENT_BINARY_DIR}/RestoreMissingInfo")
set(backupPath "${testRoot}/backup_1")
set(restorePath "${testRoot}/restored")

file(REMOVE_RECURSE "${testRoot}")
file(MAKE_DIRECTORY "${backupPath}/data")
file(WRITE "${backupPath}/data/test.txt" "Dane testowe\n")

execute_process(
    COMMAND "${SAVEPOINT_EXE}" restore "${backupPath}" "${restorePath}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE errors
    TIMEOUT 5
)

if(NOT "${result}" STREQUAL "1")
    message(FATAL_ERROR
        "Oczekiwano kodu 1, otrzymano: ${result}\n${output}\n${errors}"
    )
endif()

string(FIND "${output}" "Brak pliku backup_info.txt." messagePosition)

if(messagePosition EQUAL -1)
    message(FATAL_ERROR
        "Brakuje komunikatu o braku opisu kopii.\n${output}\n${errors}"
    )
endif()

if(EXISTS "${restorePath}")
    message(FATAL_ERROR
        "Program utworzyl folder docelowy mimo braku opisu kopii."
    )
endif()

file(REMOVE_RECURSE "${testRoot}")

message(STATUS "TEST OK: przywracanie kopii bez opisu zostalo odrzucone.")