cmake_minimum_required(VERSION 3.20)
project(SavePoint LANGUAGES CXX)
add_executable(SavePoint src/main.cpp src/DirectoryScanner.cpp src/FileCopier.cpp src/BackupInfoWriter.cpp src/PathUtils.cpp)
target_compile_features(SavePoint PRIVATE cxx_std_20)

enable_testing()

add_executable(PathUtilsTests
    tests/PathUtilsTests.cpp
    src/PathUtils.cpp
)

target_include_directories(PathUtilsTests PRIVATE src)

target_compile_features(PathUtilsTests PRIVATE cxx_std_20)

add_test(NAME PathUtilsTests COMMAND PathUtilsTests)

add_test(
    NAME RestoreMissingDestination
    COMMAND SavePoint restore test_backup
)

set_tests_properties(RestoreMissingDestination PROPERTIES
    WILL_FAIL TRUE
    TIMEOUT 5
)

add_test(
    NAME RestoreMissingInfo
    COMMAND "${CMAKE_COMMAND}"
        "-DSAVEPOINT_EXE=$<TARGET_FILE:SavePoint>"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/RestoreMissingInfo.cmake"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

set_tests_properties(
    RestoreMissingInfo
    PROPERTIES TIMEOUT 10
)

add_test(
    NAME BackupAndRestore
    COMMAND "${CMAKE_COMMAND}"
        "-DSAVEPOINT_EXE=$<TARGET_FILE:SavePoint>"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/BackupAndRestore.cmake"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

set_tests_properties(
    BackupAndRestore
    PROPERTIES TIMEOUT 15
)