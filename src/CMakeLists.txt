cmake_minimum_required(VERSION 3.2)

#
# PROJECT name and version
#
project(Logalizer VERSION 3.0 LANGUAGES CXX)

#set(CMAKE_CXX_CLANG_TIDY clang-tidy
#    -header-filter=.
#    -checks=*
#    -warnings-as-errors=*)

#
# Warnings Link this 'library' to use the following warnings
#
add_library(project_warnings INTERFACE)

if(MSVC)
  target_compile_options(project_warnings INTERFACE /W4)
else()
  target_compile_options(project_warnings
                         INTERFACE -Wall
                                   -Wextra # reasonable and standard
                                   -Wshadow # warn the user if a variable
                                            # declaration shadows one from a
                                            # parent context
                                   -Wnon-virtual-dtor # warn the user if a class
                                                      # with virtual functions
                                                      # has a non-virtual
                                                      # destructor. This helps
                                                      # catch hard to track down
                                                      # memory errors
                                   -Wold-style-cast # warn for c-style casts
                                   -Wcast-align # warn for potential performance
                                                # problem casts
                                   -Wunused # warn on anything being unused
                                   -Woverloaded-virtual # warn if you overload
                                                        # (not override) a
                                                        # virtual function
                                   -Wpedantic # warn if non-standard C++ is used
                                   -Wconversion # warn on type conversions that
                                                # may lose data
                                   -Wsign-conversion # warn on sign conversions
                                   -Wmisleading-indentation # warn if identation
                                                            # implies blocks
                                                            # where blocks do
                                                            # not exist
                                   -Wduplicated-cond # warn if if / else chain
                                                     # has duplicated conditions
                                   -Wduplicated-branches # warn if if / else
                                                         # branches have
                                                         # duplicated code
                                   -Wlogical-op # warn about logical operations
                                                # being used where bitwise were
                                                # probably wanted
                                   -Wnull-dereference # warn if a null
                                                      # dereference is detected
                                                      # -Wuseless-cast # warn if
                                                      # you perform a cast to
                                                      # the same type
                                   -Wdouble-promotion # warn if float is
                                                      # implicit promoted to
                                                      # double
                                   -Wformat=2 # warn on security issues around
                                              # functions that format output (ie
                                              # printf)
                         )
endif()

#
# Coverage
#
if(CMAKE_COMPILER_IS_GNUCC)
  option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" FALSE)

  if(ENABLE_COVERAGE)
    target_compile_options(${PROJECT_NAME} PRIVATE --coverage -O0)
  endif()
endif()

#
# CONFIGURATION
#
# The version number.
set(LOGALIZER_VERSION_MAJOR 3)
set(LOGALIZER_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings to the source code
configure_file("${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.h.in"
               "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.h")

#
# Compile and Link
#

add_executable(${PROJECT_NAME} "main.cpp" "translator.cpp")

# add the binary tree to the search path for include configure headers
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_BINARY_DIR}")

# -std=c++17 is used as cxx_std_17 is not working with mingw
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
# target_include_directories(${PROJECT_NAME} PRIVATE
# "${PROJECT_SOURCE_DIR}/lib")

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    target_link_options(${PROJECT_NAME}
                        PRIVATE
                        -static-libgcc
                        -static-libstdc++)
    target_include_directories(${PROJECT_NAME} PRIVATE "../lib/range-v3/include")
endif()

target_link_libraries(${PROJECT_NAME}
                      PRIVATE project_warnings --coverage Logalizer::config)
