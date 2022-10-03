find_package(Doxygen REQUIRED)

# Find all public headers
get_target_property(CONFIG_PUBLIC_HEADER_DIR Logalizer::config
    INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE CONFIG_PUBLIC_HEADERS ${CONFIG_PUBLIC_HEADER_DIR}/*.h)

set(DOXYGEN_INPUT_DIR "${PROJECT_SOURCE_DIR}/README.md ${PROJECT_SOURCE_DIR}/configuring.md ${PROJECT_SOURCE_DIR}/lib/config ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/docs")
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
set(DOXYFILE_CSS ${CMAKE_CURRENT_SOURCE_DIR}/doxygen/doxygen-awesome-css/doxygen-awesome.css)
set(DOXYFILE_EXCLUDE "${PROJECT_SOURCE_DIR}/lib/config/external ${PROJECT_SOURCE_DIR}/docs/doxygen")

# Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

# Doxygen won't create this for us
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

# Only regenerate Doxygen when the Doxyfile or public headers change
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                   DEPENDS ${CONFIG_PUBLIC_HEADERS}
                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                   MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                   COMMENT "Generating docs"
                   VERBATIM)

# Nice named target so we can run the job easily
add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

# Add an install target to install the docs
include(GNUInstallDirs)
install(DIRECTORY ${SPHINX_BUILD}
	DESTINATION ${CMAKE_INSTALL_DOCDIR})
