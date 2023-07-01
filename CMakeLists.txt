cmake_minimum_required(VERSION 3.5)

#
# PROJECT name and version
#
project(LogalizerApp VERSION 2.5 LANGUAGES CXX)

# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") # works

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)

add_subdirectory(lib/config)
add_subdirectory(src)

option(BUILD_DOCS "Build doxygen docs" OFF)
if(BUILD_DOCS)
    add_subdirectory(docs)
endif()

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

install(TARGETS Logalizer RUNTIME DESTINATION bin)
install(FILES $<TARGET_RUNTIME_DLLS:Logalizer> TYPE BIN)
include(CPack)
# install(CODE [[
#   function(install_library_with_deps LIBRARY)
#     file(INSTALL
#       DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
#       TYPE SHARED_LIBRARY
#       FOLLOW_SYMLINK_CHAIN
#       FILES "${LIBRARY}"
#     )
#     file(GET_RUNTIME_DEPENDENCIES
#       LIBRARIES ${LIBRARY}
#       RESOLVED_DEPENDENCIES_VAR RESOLVED_DEPS
#       UNRESOLVED_DEPENDENCIES_VAR UNRESOLVED_DEPS
#     )
#     foreach(FILE ${RESOLVED_DEPS})
#       if(NOT IS_SYMLINK ${FILE})
#         install_library_with_deps(${FILE})
#       endif()
#     endforeach()
#     foreach(FILE ${UNRESOLVED_DEPS})
#       message(STATUS "Unresolved from ${LIBRARY}: ${FILE}")
#     endforeach()
#   endfunction()
#   file(GET_RUNTIME_DEPENDENCIES
#     EXECUTABLES  $<TARGET_FILE:bin/Logalizer*>
#     RESOLVED_DEPENDENCIES_VAR RESOLVED_DEPS
#     UNRESOLVED_DEPENDENCIES_VAR UNRESOLVED_DEPS
#   )
#   foreach(FILE ${RESOLVED_DEPS})
#     install_library_with_deps(${FILE})
#   endforeach()
#   foreach(FILE ${UNRESOLVED_DEPS})
#     message(STATUS "Unresolved: ${FILE}")
#   endforeach()
# ]])
