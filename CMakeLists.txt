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