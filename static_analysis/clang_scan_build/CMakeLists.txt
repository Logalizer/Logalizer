cmake_minimum_required(VERSION 3.5)

project(cppcheck_analysis)

# Use debug build as recommended
set(CMAKE_BUILD_TYPE Debug)

# Have cmake create a compile database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add sub directories
add_subdirectory(../../src build/check/src)
add_subdirectory(../../lib/config build/check/config)
