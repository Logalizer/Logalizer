cmake_minimum_required(VERSION 3.8.2)

project(logalizer-ui)

set(SRC_FILES
  main.cpp
  MainFrameImpl.cpp
  ui/ui.cpp
  platform/application.cpp)

if(WIN32)
	add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES})
else()
  add_executable(${PROJECT_NAME} ${SRC_FILES})
endif()

find_package(wxWidgets COMPONENTS core base OPTIONAL_COMPONENTS net)
include(${wxWidgets_USE_FILE})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

