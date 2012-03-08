project(PrimeTV)
cmake_minimum_required(VERSION 2.8)
enable_language(Fortran)
enable_language(C)
###CPACK####################################################
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
set(PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})
set(CPACK_SOURCE_GENERATOR TGZ;ZIP)
# PACKAGING OPTIONS: GENERAL
set(PACKAGE_NICKNAME "primetv")
set(CPACK_SET_DESTDIR TRUE) # Enabling absolute paths for CPack (important!)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PrimeTV a tool to visualize reconciled trees.")
set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};${CMAKE_PROJECT_NAME};ALL;/")
set(CPACK_PACKAGE_NAME ${PACKAGE_NICKNAME})
IF(APPLE)
	SET(CPACK_GENERATOR "TGZ;TBZ2;OSXX11")
        set(CPACK_PACKAGE_FILE_NAME "${PACKAGE_NICKNAME}-${PACKAGE_VERSION}-mac-${TARGET_ARCH}")
        set(CPACK_SOURCE_PACKAGE_FILE_NAME  "${PACKAGE_NICKNAME}-${PACKAGE_VERSION}-mac-src")
        SET(CMAKE_OSX_SYSROOT /Developer/SDKs/MacOSX10.4u.sdk)
        SET(CMAKE_OSX_ARCHITECTURES ppc;i386)
        SET(CMAKE_FIND_ROOT_PATH /Developer/SDKs/MacOSX10.4u.sdk)
        SET(MACOSX_BUNDLE_BUNDLE_NAME Converters)
        SET(MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION})
        SET(MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION})
        SET(MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${VERSION}")
elseif(UNIX)
	set(CPACK_GENERATOR "DEB;RPM") # Distribution packaging formats ("make package"); RPM packaging can also be included later
        set(CPACK_PACKAGE_FILE_NAME "${PACKAGE_NICKNAME}-${PACKAGE_VERSION}-linux-${TARGET_ARCH}")
        set(CPACK_SOURCE_PACKAGE_FILE_NAME  "${PACKAGE_NICKNAME}-${PACKAGE_VERSION}-mac-src")
endif()

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PACKAGE_NICKNAME}-${PACKAGE_VERSION}")
set(CPACK_STRIP_FILES TRUE)
file(GLOB_RECURSE DOT_FILES_BEGIN ".*") # To be ignored by Cpack
file(GLOB_RECURSE TILD_FILES "*~*") # To be ignored by Cpack
set(CPACK_SOURCE_IGNORE_FILES "/CVS/;/.svn/;/.swp$/;cscope.*;/.git/;${CMAKE_CURRENT_BINARY_DIR}/;/.bzr/;/.settings/;${DOT_FILES_BEGIN};${TILD_FILES}")

# PACKAGING OPTIONS: DEB
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jose Fernandez <jc.fernandez.navarro@gmail.com>")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${TARGET_ARCH})
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-dev,libcairo-dev")

# PACKAGING OPTIONS: RPM
set(CPACK_RPM_PACKAGE_LICENSE "Apache 2.0 license")
set(CPACK_RPM_PACKAGE_GROUP "Jose Fernandez")
set(CPACK_RPM_PACKAGE_DEPENDS "libc6, libgcc1")
set(CPACK_RPM_PACKAGE_DEPENDS "${CPACK_RPM_PACKAGE_DEPENDS},boost-devel,")

include(CPack)
#####CPACK###################################################

###PACKAGES######################################
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
if(${QT4_FOUND})
  message(STATUS "Qt4 found")
else(${QT4_FOUND})
  message(FATAL_ERROR "Qt4 not found")
endif(${QT4_FOUND})

find_package(X11 REQUIRED)

find_package(Cairo REQUIRED)

set(Boost_USE_STATIC_LIBS   TRUE)
set(Boost_USE_MULTITHREADED TRUE)
SET (Boost_ADDITIONAL_VERSIONS "1.36" "1.36.0" "1.41" "1.41.0" "1.39" "1.39.0" "1.42.0" "1.42" "1.43.0" 
			       "1.43." "1.44.0" "1.44.0" "1.45.0" "1.45" "1.46.0" "1.46" "1.47.0" "1.47")
find_package(Boost COMPONENTS serialization program_options REQUIRED)
if(NOT Boost_FOUND)
  message(FATAL_ERROR  "Boost not found" )
endif(NOT Boost_FOUND)

####PACKAGES##################################################

##DIRECTORIES#################################################
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${Boost_INCLUDE_DIRS})
include_directories("${PROJECT_BINARY_DIR}")
include_directories( ${CMAKE_BINARY_DIR}  )
include_directories( ${CAIRO_INCLUDE_DIRS})
include_directories( ${qt4_include_dir} )
include_directories( ${X11_INCLUDE_DIR})
######DIRECTORIES#################################################


###DEFINITIONS###################################################
add_definitions( ${LIBXML2_DEFINITIONS} )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")
set(DCMAKE_SKIP_RPATH:BOOL=ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_definitions(-DQT_DLL)
add_definitions(-DQT_THREAD_SUPPORT)
add_definitions(${QT_DEFINITIONS})
###DEFINITIONS###################################################


##FLAGS###
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wreorder  -Wall -fexceptions")
if( ${CMAKE_CXX_COMPILER} MATCHES "icpc" )
    # Remove annoying Intel warnings.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd383 -wd869 -wd981")
endif( ${CMAKE_CXX_COMPILER} MATCHES "icpc" )
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -funroll-loops -DNDEBUG")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fexceptions")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
##FLAGS#############################################################

#Profile
list(APPEND CMAKE_BUILD_TYPES Profile)
set(CMAKE_C_FLAGS_PROFILE "-pg -g")
set(CMAKE_CXX_FLAGS_PROFILE "-pg -g")
set(CMAKE_EXE_LINKER_FLAGS_PROFILE "-pg")


flex_target(MyScanner ${PROJECT_SOURCE_DIR}/libraries/NHX.l ${PROJECT_SOURCE_DIR}/libraries/lexer.c COMPILE_FLAGS "-Pyytree_" )
bison_target(MyParser ${PROJECT_SOURCE_DIR}/libraries/NHXparse.y ${PROJECT_SOURCE_DIR}/libraries/NHXparse.c COMPILE_FLAGS "-d -y -p yytree_" ) 
add_flex_bison_dependency(MyScanner MyParser)

#QT4 DIRECTIVES
set(Qt_Headers windows.h canvas.h)
set(Qt_Forms primetv.ui parameters.ui)
set(Qt_Resources application.qrc)
qt4_wrap_cpp(Qt4_Headers_Moc ${Qt_Headers})
QT4_WRAP_UI(QT4_FORMS_HEADERS ${Qt_Forms})
QT4_ADD_RESOURCES(Qt4_Resources_Rcc ${Qt_Resources})
include(${QT_USE_FILE})

#Collect the files for libraries
include (libraries/CMakeLists.txt)
foreach(FILE ${FILES})
  set (Libraries ${Libraries} libraries/${FILE})
endforeach(FILE)

#set up tree parsers
set(PARSER_SOURCES libraries/NHXnode.c libraries/NHXtree.c libraries/NHXannotation.c ${FLEX_MyScanner_OUTPUTS} ${BISON_MyParser_OUTPUTS}  )

foreach(i ${PARSER_SOURCES})
 set_source_files_properties(${i} PROPERTIES LANGUAGE C)
endforeach(i)

add_library(prime STATIC ${PARSER_SOURCES} ${Libraries} ${Utils})

add_executable(primetv windows.cpp edge.cpp layoutrees.cpp mainops.cpp parameters.cpp phyltr.cpp Color.cpp Colours.cpp Coord.cc primetv.cc DrawTree_time.cc windows.cpp canvas.cpp  ${Qt4_Headers_Moc} ${QT4_FORMS_HEADERS} ${Qt4_Resources_Rcc})

target_link_libraries(primetv prime ${X11_LIBRARIES} ${QT_LIBRARIES} ${CAIRO_LIBRARIES}  ${Boost_LIBRARIES}  m )

#TODO : take into consideration when using windows, make use of globals
install(TARGETS primetv DESTINATION bin)
install(FILES ${PROJECT_SOURCE_DIR}/images/header.png DESTINATION /usr/share/primetv PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
