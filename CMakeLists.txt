###############################################################################
# NAME AND VERSION
###############################################################################
project(primetv2)
cmake_minimum_required(VERSION 2.8.9)
enable_language(C)
enable_language(CXX)

set(FULL_BUILD TRUE)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

include(options.cmake)
 
add_subdirectory(src)
