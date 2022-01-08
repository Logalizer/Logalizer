# Set the project name
project(config LANGUAGES CXX)

#set(CMAKE_CXX_CLANG_TIDY clang-tidy
#    -header-filter=.
#    -checks=*
#    -warnings-as-errors=*)

set(JSON_BuildTests OFF CACHE BOOL "disable JSON Tests" FORCE)
set(JSON_Install OFF CACHE BOOL "disable JSON Install" FORCE)
# add_subdirectory(external/json)

add_library(${PROJECT_NAME} STATIC
            "configparser.cpp"
            "configparser.h"
            "config_types.h"
            "jsonconfigparser.cpp"
            "jsonconfigparser.h")

add_library(Logalizer::config ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
# target_compile_options(config PRIVATE -std=c++17 -O3)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# -std=c++17 is used as cxx_std_17 is not working with mingw
# target_compile_features( config PRIVATE cxx_std_17)

# find_package(nlohmann_json) target_link_libraries (config PUBLIC nlohmann_json
# yaml-cpp) target_link_libraries (config PRIVATE nlohmann_json)
