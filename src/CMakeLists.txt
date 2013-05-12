###############################################################################
# PREPARING TO INSTALL
###############################################################################

# PRINT VARIABLES
message( STATUS )
message( STATUS
"-------------------------------------------------------------------------------"
)
message( STATUS "Building PrimeTV2:" )
message( STATUS "change a configuration variable with: cmake -D<Variable>=<Value>" )
message( STATUS "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
message( STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )
message( STATUS "CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}" )
message( STATUS "TARGET_ARCH = ${TARGET_ARCH}" )
message( STATUS
"-------------------------------------------------------------------------------"
)
message( STATUS )

###PROJECT and GLOBAL VARIABLES####################################################

SET(APPLICATION_NAME "PrimeTV2 : Phylogenetic trees viewer")

#Cpack stuff
set(LICENSE_FILE "LICENSE")
set(README_FILE "README")
set(PROJECT_VERSION_MAJOR "0")
set(PROJECT_VERSION_MINOR "1")
set(PROJECT_VERSION_PATCH "0")
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(PROJECT_VENDOR "PrimeTV2")
set(PROJECT_COPYRIGHT_YEAR "2013")
set(PROJECT_DOMAIN_FIRST "primetv2")
set(PROJECT_DOMAIN_SECOND "com")
set(PROJECT_DOMAIN "${PROJECT_DOMAIN_FIRST}.${PROJECT_DOMAIN_SECOND}")

# install locations
if(APPLE)
  if(NOT ${CMAKE_INSTALL_PREFIX})
    set(CMAKE_INSTALL_PREFIX ".")
  endif()
  set(EXECUTABLE "${PROJECT_NAME}.app")
  set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${EXECUTABLE}/Contents/bin")
  set(DOC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${EXECUTABLE}/Contents/docs")
  set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${EXECUTABLE}/Contents/include")
  set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${EXECUTABLE}/Contents/lib")
  set(APPS "${CMAKE_INSTALL_PREFIX}/${EXECUTABLE}")
  set(PLUGINS "${CMAKE_INSTALL_PREFIX}/${EXECUTABLE}/Contents/Plugins")
  set(QTCONF "${CMAKE_INSTALL_PREFIX}/${EXECUTABLE}/Contents/Resources")
elseif(WIN32 OR MINGW)
  if(NOT ${CMAKE_INSTALL_PREFIX})
    set(CMAKE_INSTALL_PREFIX ".")
  endif()
  set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")
  set(DOC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/docs")
  set(EXECUTABLE "${PROJECT_NAME}.exe")
  set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include")
  set(LIB_INSTALL_DIR "${BIN_INSTALL_DIR}")
  set(APPS "${CMAKE_INSTALL_PREFIX}/bin/${EXECUTABLE}")
  set(PLUGINS ${BIN_INSTALL_DIR})
  set(QTCONF ${BIN_INSTALL_DIR})
elseif(UNIX)
  if(NOT ${CMAKE_INSTALL_PREFIX})
    set(CMAKE_INSTALL_PREFIX "bin")
  endif()
  string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWERCASE)
  set(BIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")
  set(DOC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/doc/${PROJECT_NAME_LOWERCASE}/")
  set(EXECUTABLE "${PROJECT_NAME_LOWERCASE}${CMAKE_EXECUTABLE_SUFFIX}")
  set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME_LOWERCASE}")
  set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME_LOWERCASE}")
  set(APPS "${CMAKE_INSTALL_PREFIX}/bin/${EXECUTABLE}")
  set(PLUGINS ${BIN_INSTALL_DIR})
  set(QTCONF ${BIN_INSTALL_DIR})
endif()

set(ICONS_DIR "${PROJECT_SOURCE_DIR}/resources/icons")

#location of some files needed by primetv
set(HEADER_LOCATION "${CMAKE_INSTALL_PREFIX}/share/primetv/header.png")
add_definitions( -DHAS_HEADER="${HEADER_LOCATION}")


###PACKAGES######################################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake" "${PROJECT_SOURCE_DIR}/cmake/qt5")
set(CMAKE_PREFIX_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake" "${PROJECT_SOURCE_DIR}/cmake/qt5")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

use_qt5lib(Qt5Widgets)
use_qt5lib(Qt5Core)
use_qt5lib(Qt5Gui)
use_qt5lib(Qt5PrintSupport)

#useful qt5 locations
SET(QT_BINARY_DIR ${_qt5Core_install_prefix}/bin)
SET(QT_LIBRARY_DIR ${_qt5Core_install_prefix}/lib)
SET(QT_PLUGINS_DIR ${_qt5Core_install_prefix}/plugins)
SET(QT_VERSION_MAJOR ${Qt5Core_VERSION_MAJOR})
SET(QT_VERSION_MINOR ${Qt5Core_VERSION_MINOR})
SET(QT_VERSION_PATCH ${Qt5Core_VERSION_PATCH})

message(STATUS "Qt5 binary dir : ${QT_BINARY_DIR}")
message(STATUS "Qt5 library dir : ${QT_LIBRARY_DIR}")
message(STATUS "Qt5 plugins dir : ${QT_PLUGINS_DIR}")
    
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED OFF) 
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost REQUIRED)  
include_directories(${Boost_INCLUDE_DIRS})

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(Cairo REQUIRED)

####PACKAGES##################################################

##DIRECTORIES#################################################
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${Boost_INCLUDE_DIRS}")
include_directories("${PROJECT_BINARY_DIR}/src")
include_directories("${CMAKE_BINARY_DIR}")
include_directories("${PROJECT_BINARY_DIR}/src/libraries")
include_directories("${CAIRO_INCLUDE_DIR}")
######DIRECTORIES#################################################


###DEFINITIONS###################################################

add_definitions(-DPROJECT_VERSION=\"${PROJECT_VERSION}\")

###DEFINITIONS###################################################


flex_target(MyScanner libraries/NHX.l libraries/lexer.c COMPILE_FLAGS "-Pyytree_" )
bison_target(MyParser libraries/NHXparse.y libraries/NHXparse.c COMPILE_FLAGS "-d -y -p yytree_" ) 
add_flex_bison_dependency(MyScanner MyParser)

#QT5 DIRECTIVES
set(Qt_Headers windows.h canvas.h)
set(FORMS "${PROJECT_SOURCE_DIR}/resources/primetv.ui" "${PROJECT_SOURCE_DIR}/resources/parameters.ui")
set(RESOURCES "${PROJECT_SOURCE_DIR}/resources/application.qrc")

qt5_wrap_ui(QT_FORMS ${FORMS})
qt5_add_resources(QT_RESOURCES ${RESOURCES})

#Collect the files for libraries
include (libraries/CMakeLists.txt)
foreach(FILE ${FILES})
  set (Libraries ${Libraries} libraries/${FILE})
endforeach(FILE)

#set up tree parsers
set(PARSER_SOURCES libraries/NHXnode.c libraries/NHXtree.c 
		   libraries/NHXannotation.c ${FLEX_MyScanner_OUTPUTS} ${BISON_MyParser_OUTPUTS}  )

set(MAIN_SOURCES windows.cpp edge.cpp layoutrees.cpp mainops.cpp 
		 parameters.cpp phyltr.cpp Color.cpp Colours.cpp 
		 DrawTree_time.cc canvas.cpp Layout.cpp 
		 ${QT_FORMS} ${QT_RESOURCES})
		  
foreach(i ${PARSER_SOURCES})
 set_source_files_properties(${i} PROPERTIES LANGUAGE C)
endforeach(i)

add_library(primetvlib STATIC ${MAIN_SOURCES} ${PARSER_SOURCES} ${Libraries} )

add_executable(${PROJECT_NAME} primetv.cc)
qt5_use_modules(${PROJECT_NAME} Widgets Core Gui PrintSupport) 
target_link_libraries(${PROJECT_NAME} primetvlib ${CAIRO_LIBRARIES} ${Boost_LIBRARIES} ) #${OPENGL_LIBRARIES} ${X11_LIBRARIES}  ) 

if(APPLE)
  set_property(TARGET ${PROJECT_NAME} PROPERTY LINK_SEARCH_END_STATIC ON)
endif(APPLE)

#TODO : take into consideration when using windows, make use of globals
install(FILES ${PROJECT_SOURCE_DIR}/resources/images/header.png 
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/primetv 
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

install(TARGETS ${PROJECT_NAME} 
	BUNDLE DESTINATION . COMPONENT Runtime
	RUNTIME DESTINATION bin COMPONENT Runtime)

##BUILDING AND INSTALLING #############################################################

if(APPLE)
  
  include(InstallRequiredSystemLibraries)
  
  # install a qt.conf file
  ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/resources/qt.conf
                                                      ${QTCONF}/qt.conf)

  message(STATUS "Deploying MAC BUNDLE in ${APPS}/Contents/MacOS")                                                

  # Clean up our main executable
  OS_X_CLEAN_UP_FILE_WITH_QT_DEPENDENCIES(${APPS}/Contents/MacOS ${PROJECT_NAME}
                                          QtWidgets QtCore QtGui QtPrintSupport)

  # Deploy our Qt dependencies
  OS_X_DEPLOY_QT_LIBRARIES(QtWidgets QtCore QtGui QtQuick QtV8 QtQml QtOpenGL QtSql QtPrintSupport)

  # Deploy some plugin
  OS_X_DEPLOY_QT_PLUGIN(imageformats qgif qico qjpeg qmng qsvg qtga qtiff qwbmp)
  OS_X_DEPLOY_QT_PLUGIN(platforms qcocoa)
  
elseif(WIN32 OR MINGW)
  #to fill here
else()
  #to fill here
endif()

# PACKAGING OPTIONS: GENERAL
set(PACKAGE_NICKNAME "primetv")
set(CPACK_SET_DESTDIR TRUE) # Enabling absolute paths for CPack (important!)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PrimeTV a tool to visualize reconciled trees.")
set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BINARY_DIR};${CMAKE_PROJECT_NAME};ALL;/")
set(CPACK_PACKAGE_NAME ${PACKAGE_NICKNAME})
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "${PROJECT_VENDOR}")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/${README_FILE}")

if(WIN32)  ## consider using external nsi script
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "${PROJECT_NAME}")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME} ${PROJECT_VERSION}")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/${LICENSE_FILE}")
  set(CPACK_NSIS_EXECUTABLES_DIRECTORY "${BIN_INSTALL_DIR}")
  set(CPACK_NSIS_MUI_ICON "${ICONS_DIR}/NSIS.ico")
  set(CPACK_PACKAGE_ICON "${ICONS_DIR}\\\\NSISHeader.bmp")
  set(CPACK_NSIS_URL_INFO_ABOUT "http://${PROJECT_DOMAIN}")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
  set(CPACK_NSIS_MENU_LINKS "${LICENSE_FILE}" "License" "${README_FILE}" "Readme")
  set(CPACK_NSIS_MUI_FINISHPAGE_RUN "${CPACK_NSIS_INSTALLED_ICON_NAME}")
elseif(APPLE)
  set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_NAME} ${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_COPYRIGHT "${PROJECT_COPYRIGHT_YEAR} ${PROJECT_VENDOR}")
  set(MACOSX_BUNDLE_ICON_FILE "icon.png")
  set(MACOSX_BUNDLE_GUI_IDENTIFIER "${PROJECT_DOMAIN_SECOND}.${PROJECT_DOMAIN_FIRST}")
  set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")
  set(MACOSX_BUNDLE_RESOURCES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Contents/Resources")
  set(MACOSX_BUNDLE_ICON "${ICONS_DIR}/${MACOSX_BUNDLE_ICON_FILE}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${MACOSX_BUNDLE_RESOURCES})
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MACOSX_BUNDLE_ICON} ${MACOSX_BUNDLE_RESOURCES})
  set(CPACK_GENERATOR "DragNDrop")
  set(CPACK_DMG_FORMAT "UDBZ")
  set(CPACK_DMG_VOLUME_NAME "${PROJECT_NAME}")
  set(CPACK_SYSTEM_NAME "OSX")
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
#   set(CPACK_PACKAGE_ICON "${ICONS_DIR}/DMG.icns")
#   set(CPACK_DMG_DS_STORE "${ICONS_DIR}/DMGDSStore")
#   set(CPACK_DMG_BACKGROUND_IMAGE "${ICONS_DIR}/DMGBackground.png")
elseif(UNIX)
  set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_PACKAGE_DEBUG ON)
  set(CPACK_DEBIAN_PACKAGE_DEPENDS ${PACKAGE_REQUIRES})
#   set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt4-gui ( >= 4.6 ), libqt4-core ( >= 4.6 ), libqt4-network ( >= 4.6 )")
  set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR} <${CPACK_PACKAGE_CONTACT}>")
  set(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
  set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
  set(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
  set(CPACK_SET_DESTDIR TRUE)
endif()

include(CPack)
#####CPACK###################################################
