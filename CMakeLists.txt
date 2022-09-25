cmake_minimum_required(VERSION 3.5)

#
# PROJECT name and version
#
project(LogalizerApp VERSION 2.5 LANGUAGES CXX)

# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

add_subdirectory(lib/config)
add_subdirectory(src)
add_subdirectory(uiapp)
# add_subdirectory(docs)

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

install(TARGETS Logalizer DESTINATION bin)
