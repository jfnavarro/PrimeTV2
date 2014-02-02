###############################################################################
# NAME AND VERSION
###############################################################################
project(primetv)
cmake_minimum_required(VERSION 2.8.9)
enable_language(CXX C)

set(APPLICATION_NAME "PrimeTV : Phylogenetic trees viewer")

#Cpack stuff
set(LICENSE_FILE "LICENSE")
set(README_FILE "README")
set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "2")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_VENDOR "PrimeTV")
set(PROJECT_COPYRIGHT_YEAR "2013")
set(PROJECT_DOMAIN_FIRST "primetv")
set(PROJECT_DOMAIN_SECOND "com")
set(PROJECT_DOMAIN "${PROJECT_DOMAIN_FIRST}.${PROJECT_DOMAIN_SECOND}")

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  if(WIN32)
    cmake_policy(SET CMP0020 NEW)
  endif(WIN32)
endif(COMMAND cmake_policy)

# Some in-house CMake functions/macros
include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)

configure_file(${PROJECT_SOURCE_DIR}/cmake/options_cmake.h.in ${PROJECT_BINARY_DIR}/options_cmake.h )
include_directories(${PROJECT_BINARY_DIR})

add_subdirectory(src)
