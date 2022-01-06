project(Logalizer_test VERSION 1.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    main.cpp
    jsonconfigparser.cpp
    translator.cpp ../src/translator.cpp
    path_variable_utils.cpp)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
                      PRIVATE Logalizer::config)
target_compile_options(${PROJECT_NAME} PRIVATE -std=c++17)
target_include_directories(${PROJECT_NAME} PRIVATE ../src)
