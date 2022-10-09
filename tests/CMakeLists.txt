project(Logalizer_test VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)

# Create OBJECT_DIR variable
set(OBJECT_DIR ${CMAKE_BINARY_DIR}/tests/CMakeFiles/${PROJECT_NAME}.dir)
message("-- Object files will be output to: ${OBJECT_DIR}")

add_executable(${PROJECT_NAME}
    main.cpp
    jsonconfigparser.cpp
    translator.cpp ../src/translator.cpp)


target_link_libraries(${PROJECT_NAME}
                      PRIVATE Logalizer::config)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PRIVATE ../src)

# Create the gcov target. Run coverage tests with 'make gcov'
add_custom_target(gcov
    COMMAND mkdir -p coverage
    COMMAND ${CMAKE_MAKE_PROGRAM} test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
add_custom_command(TARGET gcov
    COMMAND echo "=================== GCOV ===================="
    COMMAND gcov -b ${CMAKE_SOURCE_DIR}/*.cpp -o ${OBJECT_DIR} > CoverageSummary.tmp
    COMMAND cat CoverageSummary.tmp
    COMMAND echo "-- Coverage files have been output to ${CMAKE_BINARY_DIR}/coverage"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage  # Need separate command for this line
    )
add_dependencies(gcov ${PROJECT_NAME})

# Make sure to clean up the coverage folder
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES coverage)

# Create the gcov-clean target. This cleans the build as well as generated 
# .gcda and .gcno files.
add_custom_target(scrub
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND rm -f ${OBJECT_DIR}/*.gcno
    COMMAND rm -f ${OBJECT_DIR}/*.gcda
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

# Generate LCOV report
add_custom_target(lcov
    COMMAND echo "=================== LCOV ===================="
    COMMAND lcov --capture --directory ${OBJECT_DIR} --output-file coverage.info
    COMMAND lcov --remove coverage.info --output-file coverage.info '${CMAKE_SOURCE_DIR}/lib/config/external/*' '${CMAKE_SOURCE_DIR}/tests/*' '9/*' '/usr/include/*'
    COMMAND genhtml coverage.info --output-directory lcov_coverage_report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

# Testing
enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
