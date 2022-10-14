project(Logalizer_test VERSION 1.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME}
    main.cpp
    jsonconfigparser.cpp
    translator.cpp ../src/translator.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Logalizer::config)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PRIVATE ../src)

# Test Coverage
option(BUILD_COVERAGE "Generate lcov coverage report" OFF)
if(BUILD_COVERAGE)
  include(CodeCoverage)
  append_coverage_compiler_flags_to_target(${PROJECT_NAME})
  set(COVERAGE_EXCLUDES
           EXCLUDE "/usr/include/*"
           "${CMAKE_SOURCE_DIR}/lib/config/external/*"
           "${CMAKE_SOURCE_DIR}/tests/*"
           )
  setup_target_for_coverage_lcov(
           NAME coverage
           EXECUTABLE ${PROJECT_NAME})
endif()

# Testing
enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
