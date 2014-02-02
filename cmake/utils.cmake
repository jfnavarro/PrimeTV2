MACRO(INITIALISE_PROJECT)

    #SET(CMAKE_VERBOSE_MAKEFILE ON)
    SET(CMAKE_INCLUDE_CURRENT_DIR ON)

    IF(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
        MESSAGE("Building a debug version...")
        SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -DDEBUG")
        # Make sure that debugging is on for Qt
        ADD_DEFINITIONS(-DQT_DEBUG)
    ELSE()
        MESSAGE("Building a release version...")
        # Make sure that debugging is off for Qt
        ADD_DEFINITIONS(-DQT_NO_DEBUG_OUTPUT)
        ADD_DEFINITIONS(-DQT_NO_DEBUG)
    ENDIF()

    # Reduce the number of warnings
    # Remove "warning: multi-character character constant"
    OPTION(TREAT_WARNINGS_AS_ERRORS "Treat warnings as errors" ON)

    IF(WIN32)
        #TODO
    ELSE()
        IF(TREAT_WARNINGS_AS_ERRORS)
            SET(WARNING_ERROR "-Werror")
        ENDIF(TREAT_WARNINGS_AS_ERRORS)
        SET(DISABLED_WARNINGS "-Wno-float-equal -Wno-shadow -Wno-unreachable-code -Wno-switch-enum -Wno-unused-function -Wno-switch-default")
        SET(DISABLED_WARNINGS_DEBUG "-Wno-float-equal -Wno-shadow -Wno-unreachable-code -Wno-switch-enum -Wno-unused-function -Wno-switch-default")
        SET(EXTRA_WARNINGS_C "-Wall -Wunused-variable -Wextra  -Wformat-nonliteral -Wformat -Wreturn-type -Wempty-body -Wdeprecated -Wdisabled-optimization -Winline -Wpacked -Wuninitialized -Wcast-align -Wcast-qual -Wswitch -Wsign-compare -pedantic-errors")
        SET(EXTRA_WARNINGS_CXX "${EXTRA_WARNINGS_C} -Woverloaded-virtual -Wnon-virtual-dtor -fuse-cxa-atexit -ffor-scope") #-Weffc++
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            SET(EXTRA_WARNINGS_CXX "${EXTRA_WARNINGS_CXX} -Wpedantic -Wint-to-void-pointer-cast -Wold-style-cast -Wredundant-decls")
        endif()
    ENDIF()

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_WARNINGS_CXX} ${DISABLED_WARNINGS} ${WARNING_ERROR}")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${EXTRA_WARNINGS_CXX} ${DISABLED_WARNINGS_DEBUG} ${WARNING_ERROR}")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_WARNINGS_C} ${DISABLED_WARNINGS} ${WARNING_ERROR}")
    SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${EXTRA_WARNINGS_C} ${DISABLED_WARNINGS_DEBUG} ${WARNING_ERROR}")

    #enable c++11
    check_for_cxx11_compiler(CXX11_COMPILER)

    # If a C++11 compiler is available, then set the appropriate flags
    if(CXX11_COMPILER)
        enable_cxx11()
    else()
        message(FATAL_ERROR "Your compiler does not support c++11, UPDATE!!")
    endif()

    if(APPLE)
        set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
        set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
        set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.7 -stdlib=libc++")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -mmacosx-version-min=10.7 -stdlib=libc++")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmacosx-version-min=10.7 -stdlib=libc++")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -mmacosx-version-min=10.7 -stdlib=libc++")
    endif()

ENDMACRO()

MACRO(use_qt5lib qt5lib)
    find_package(${qt5lib} REQUIRED)
    include_directories(${${qt5lib}_INCLUDE_DIRS})
    add_definitions(${${qt5lib}_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${${qt5lib}_EXECUTABLE_COMPILE_FLAGS}")
ENDMACRO()

# Determines whether or not the compiler supports C++11
macro(check_for_cxx11_compiler _VAR)
    message(STATUS "Checking for C++11 compiler")
    set(${_VAR})
    if ((MSVC AND MSVC10) OR
       (CMAKE_COMPILER_IS_GNUCXX AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.6) OR
       (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.1))
        set(${_VAR} 1)
        message(STATUS "Checking for C++11 compiler - available")
    else()
        message(STATUS "Checking for C++11 compiler - unavailable")
    endif()
endmacro()

# Sets the appropriate flag to enable C++11 support
macro(enable_cxx11)
    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++0x")
    endif()
endmacro()

