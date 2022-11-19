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
            "jsonconfigparser.h"
            "path_variable_utils.cpp")

add_library(Logalizer::config ALIAS ${PROJECT_NAME})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED) # fast-cpp-csv-parser needs this 
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json::nlohmann_json)

find_path(FAST_CPP_CSV_PARSER_INCLUDE_DIRS "csv.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${FAST_CPP_CSV_PARSER_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
# target_compile_options(config PRIVATE -std=c++17 -O3)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
