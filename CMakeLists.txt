cmake_minimum_required(VERSION 3.20)
project(SavePoint LANGUAGES CXX)
add_executable(SavePoint src/main.cpp src/DirectoryScanner.cpp src/FileCopier.cpp src/BackupInfoWriter.cpp src/PathUtils.cpp)
target_compile_features(SavePoint PRIVATE cxx_std_20)