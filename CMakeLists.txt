project(PrimeTV)
cmake_minimum_required(VERSION 2.8)
enable_language(Fortran)
enable_language(C)
###CPACK####################################################
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "9")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
set(PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})
set(CPACK_GENERATOR RPM;DEB)
set(CPACK_SOURCE_GENERATOR TGZ;ZIP)
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
if(CAIRO_FOUND)
	MESSAGE(STATUS "Found Cairo")
	set(Cairo "${CAIRO_LIBRARY_DIRS}/libcairo.so")
else(CAIRO_FOUND)
	message(FATAL_ERROR "Cairo not found")
endif(CAIRO_FOUND)

set(Boost_USE_STATIC_LIBS   TRUE)
set(Boost_USE_MULTITHREADED TRUE)
SET(Boost_ADDITIONAL_VERSIONS "1.40" "1.42")

find_package(Boost 1.40.0  COMPONENTS serialization program_options REQUIRED)
if(NOT Boost_FOUND)
  message(FATAL_ERROR  "Boost not found" )
endif(NOT Boost_FOUND)

#find_package(LibXml2 REQUIRED)


####PACKAGES##################################################

##DIRECTORIES#################################################
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${Boost_INCLUDE_DIRS})
#include_directories( ${LIBXML2_INCLUDE_DIR} )
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

target_link_libraries(primetv prime ${X11_LIBRARIES} ${QT_LIBRARIES} ${Cairo}  ${Boost_LIBRARIES} ${LIBXML2_LIBRARIES}  m )

install(TARGETS primetv DESTINATION bin)
