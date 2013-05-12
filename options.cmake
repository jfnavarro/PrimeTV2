# Global build options 
# These propagate to common/options_cmake.h

macro(use_qt5lib qt5lib)
	find_package(${qt5lib} REQUIRED)
	include_directories(${${qt5lib}_INCLUDE_DIRS})
	add_definitions(${${qt5lib}_DEFINITIONS})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${qt5lib}_EXECUTABLE_COMPILE_FLAGS}")
endmacro()

MACRO(OS_X_CLEAN_UP_FILE_WITH_QT_DEPENDENCIES DIRNAME FILENAME)
    # Strip the Qt file of all local symbols

    SET(FULL_FILENAME ${DIRNAME}/${FILENAME})

    IF(NOT DEBUG_MODE)
        ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
                           COMMAND strip -x ${FULL_FILENAME})
    ENDIF()

    # Clean up the Qt file's id

    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND install_name_tool -id ${FILENAME}
                                                     ${FULL_FILENAME})

    # Make sure that the Qt file refers to our embedded version of its Qt
    # dependencies

    FOREACH(DEPENDENCY ${ARGN})
        SET(DEPENDENCY_FILENAME ${DEPENDENCY}.framework/Versions/${QT_VERSION_MAJOR}/${DEPENDENCY})

        ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
                           COMMAND install_name_tool -change ${QT_LIBRARY_DIR}/${DEPENDENCY_FILENAME}
                                                             @executable_path/../Frameworks/${DEPENDENCY_FILENAME}
                                                             ${FULL_FILENAME})
    ENDFOREACH()
ENDMACRO()

MACRO(OS_X_DEPLOY_QT_LIBRARIES)
    FOREACH(LIBRARY_NAME ${ARGN})
        # Deploy the Qt library itself

        SET(QT_FRAMEWORK_DIR ${LIBRARY_NAME}.framework/Versions/${QT_VERSION_MAJOR})

        OS_X_DEPLOY_QT_FILE(${QT_LIBRARY_DIR}/${QT_FRAMEWORK_DIR}
                            ${APPS}/Contents/Frameworks/${QT_FRAMEWORK_DIR}
                            ${LIBRARY_NAME})
    ENDFOREACH()
ENDMACRO()

MACRO(OS_X_DEPLOY_QT_PLUGIN PLUGIN_CATEGORY)
    FOREACH(PLUGIN_NAME ${ARGN})
        # Deploy the Qt plugin itself

        OS_X_DEPLOY_QT_FILE(${QT_PLUGINS_DIR}/${PLUGIN_CATEGORY}
                            ${APPS}/Contents/PlugIns/${PLUGIN_CATEGORY}
                            ${CMAKE_SHARED_LIBRARY_PREFIX}${PLUGIN_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX})
    ENDFOREACH()
ENDMACRO()

MACRO(OS_X_DEPLOY_QT_FILE ORIG_DIRNAME DEST_DIRNAME FILENAME)
    # Copy the Qt file itself

    SET(ORIG_FILENAME ${ORIG_DIRNAME}/${FILENAME})
    
    ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${ORIG_FILENAME}
                                                        ${DEST_DIRNAME}/${FILENAME})

    # Retrieve the Qt file's Qt dependencies

    OS_X_QT_DEPENDENCIES(${ORIG_FILENAME} DEPENDENCIES)

    # Clean up the Qt file

    OS_X_CLEAN_UP_FILE_WITH_QT_DEPENDENCIES(${DEST_DIRNAME} ${FILENAME} ${DEPENDENCIES})
    
ENDMACRO()

MACRO(OS_X_QT_DEPENDENCIES FILENAME QT_DEPENDENCIES)
    # Retrieve the file's full-path Qt dependencies as a list

    SET(QT_LIBRARY_DIR_FOR_GREP "\t${QT_LIBRARY_DIR}/")

    EXECUTE_PROCESS(COMMAND otool -L ${FILENAME}
                    COMMAND grep ${QT_LIBRARY_DIR_FOR_GREP}
                    OUTPUT_VARIABLE RAW_QT_DEPENDENCIES)

    STRING(REPLACE "\n" ";" RAW_QT_DEPENDENCIES "${RAW_QT_DEPENDENCIES}")

    # Extract and return the Qt depencies as a list

    SET(${QT_DEPENDENCIES})

    FOREACH(RAW_QT_DEPENDENCY ${RAW_QT_DEPENDENCIES})
        STRING(REPLACE ${QT_LIBRARY_DIR_FOR_GREP} "" RAW_QT_DEPENDENCY "${RAW_QT_DEPENDENCY}")
        STRING(REGEX REPLACE "\\.framework.*$" "" QT_DEPENDENCY "${RAW_QT_DEPENDENCY}")

        LIST(APPEND ${QT_DEPENDENCIES} ${QT_DEPENDENCY})
    ENDFOREACH()
ENDMACRO()


if(APPLE)
  SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
  SET (CMAKE_OSX_ARCHITECTURES ${TARGET_ARCH})
  OPTION(STATIC "build a static binaries. This is mainly intended for the win32-package and the MACOSX-bundle." ON )
endif(APPLE)

# Detect machine architecture, on UNIX:
if(UNIX AND NOT APPLE AND NOT DEFINED ${TARGET_ARCH})
	execute_process(COMMAND dpkg --print-architecture
		OUTPUT_VARIABLE TARGET_ARCH
                OUTPUT_STRIP_TRAILING_WHITESPACE)
	message(STATUS "Architecture: ${TARGET_ARCH}")
endif(UNIX AND NOT APPLE AND NOT DEFINED ${TARGET_ARCH})
if(APPLE AND NOT DEFINED ${TARGET_ARCH})
	execute_process(COMMAND uname -m
		OUTPUT_VARIABLE TARGET_ARCH
                OUTPUT_STRIP_TRAILING_WHITESPACE)
	message(STATUS "Architecture: ${TARGET_ARCH}")
endif(APPLE AND NOT DEFINED ${TARGET_ARCH})

IF (WIN32)
    SET (QT_USE_QTMAIN TRUE)
ENDIF (WIN32)

if(WIN32 OR MINGW OR APPLE)
  OPTION(STATIC "build a static binaries. This is mainly intended for the win32-package and the MACOSX-bundle." ON )
else()
  OPTION(STATIC "build a static binaries. This is mainly intended for the win32-package and the MACOSX-bundle." OFF )
endif()

IF(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.5")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmacosx-version-min=10.5")
  set(LINK_FLAGS "-mmacosx-version-min=10.5")
ENDIF(APPLE)

#debug variables
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
elseif(NOT CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]")
  add_definitions(-DQT_NO_DEBUG_OUTPUT)
  set(CMAKE_BUILD_RELEASE TRUE)
else()
  set(CMAKE_BUILD_RELEASE FALSE)
endif()

#Reduce the number of warnings
#Remove "warning: multi-character character constant"

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/options_cmake.h.in   ${CMAKE_CURRENT_BINARY_DIR}/common/options_cmake.h @ONLY )
