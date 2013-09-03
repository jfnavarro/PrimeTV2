###############################################################################
# PREPARING TO INSTALL
###############################################################################

# PRINT VARIABLES
message( STATUS )
message( STATUS
"-------------------------------------------------------------------------------"
)
message( STATUS "Building PrimeTV:" )
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

# Initialise the project
initialise_project()
#location of the icons
set(ICONS_DIR "${PROJECT_SOURCE_DIR}/resources/icons")
#location of some files needed by primetv
set(HEADER_LOCATION "${CMAKE_INSTALL_PREFIX}/share/primetv/header.png")
add_definitions(-DHAS_HEADER="${HEADER_LOCATION}")

# Organize projects into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

###PACKAGES######################################

# put everything that is generated into the binary dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BINARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# location of home made cmake macros
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake" )
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${PROJECT_SOURCE_DIR}/cmake" )

#find qt libraries
use_qt5lib(Qt5Widgets)
use_qt5lib(Qt5Core)
use_qt5lib(Qt5Gui)
use_qt5lib(Qt5PrintSupport)

#find boost    
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON) 
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost REQUIRED COMPONENTS system program_options)  
#find bison flex and cairo
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(Cairo REQUIRED)

####PACKAGES##################################################

##DIRECTORIES#################################################
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CAIRO_INCLUDE_DIR})
######DIRECTORIES#################################################


###DEFINITIONS###################################################

add_definitions(-DPROJECT_VERSION=\"${PROJECT_VERSION}\")

###DEFINITIONS###################################################

bison_target(MyParser "${PROJECT_SOURCE_DIR}/src/libraries/NHXparse.y" 
                      "${PROJECT_SOURCE_DIR}/src/libraries/NHXparse.c" 
                      COMPILE_FLAGS "-d -y -p yytree_" ) 
flex_target(MyScanner "${PROJECT_SOURCE_DIR}/src/libraries/NHX.l" 
                      "${PROJECT_SOURCE_DIR}/src/libraries/lexer.c" 
                      COMPILE_FLAGS "-Pyytree_" )
add_flex_bison_dependency(MyScanner MyParser)

#QT5 DIRECTIVES
set(FORMS "${PROJECT_SOURCE_DIR}/resources/ui/primetv.ui" 
          "${PROJECT_SOURCE_DIR}/resources/ui/parameters.ui")
          
set(RESOURCES "${PROJECT_SOURCE_DIR}/resources/application.qrc")

qt5_wrap_ui(QT_FORMS ${FORMS})
qt5_add_resources(QT_RESOURCES ${RESOURCES})

#Collect the files for libraries
include (libraries/CMakeLists.txt)
foreach(FILE ${FILES})
  set (Libraries ${Libraries} libraries/${FILE})
endforeach(FILE)

#set up tree parsers
set(PARSER_SOURCES "${PROJECT_SOURCE_DIR}/src/libraries/NHXnode.c" 
                   "${PROJECT_SOURCE_DIR}/src/libraries/NHXtree.c" 
                   "${PROJECT_SOURCE_DIR}/src/libraries/NHXannotation.c" 
                   "${BISON_MyParser_OUTPUTS}" 
                   "${FLEX_MyScanner_OUTPUTS}")

set(MAIN_SOURCES windows.cpp edge.cpp layoutrees.cpp mainops.cpp 
                 parameters.cpp phyltr.cpp Color.cpp Colours.cpp 
                 DrawTree_time.cc canvas.cpp GraphicView.cpp)
		  
foreach(i ${PARSER_SOURCES})
 set_source_files_properties(${i} PROPERTIES LANGUAGE C)
endforeach(i)

add_library(primetvlib STATIC ${PARSER_SOURCES} ${Libraries})

if(WIN32)
  add_executable(${PROJECT_NAME} WIN32 primetv.cc ${MAIN_SOURCES} ${QT_FORMS} ${QT_RESOURCES})
elseif(APPLE)
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE primetv.cc ${MAIN_SOURCES} ${QT_FORMS} ${QT_RESOURCES})
else()
  add_executable(${PROJECT_NAME} primetv.cc ${MAIN_SOURCES} ${QT_FORMS} ${QT_RESOURCES})
endif()

qt5_use_modules(${PROJECT_NAME} Widgets Core Gui PrintSupport) 
target_link_libraries(${PROJECT_NAME} primetvlib ${CAIRO_LIBRARIES} ${Boost_LIBRARIES}) 

##################################################################INSTALLATION###########################################################################

# PACKAGING OPTIONS: GENERAL
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "${PROJECT_VENDOR}")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_SOURCE_DIR}/WELCOME")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

if(UNIX AND NOT APPLE)

    # use, i.e. don't skip the full RPATH for the build tree
    set(CMAKE_SKIP_BUILD_RPATH FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/plugins")

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    # the RPATH to be used when installing, but only if it's not a system directory
    list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
    if("${isSystemDir}" STREQUAL "-1")
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    endif("${isSystemDir}" STREQUAL "-1")

    # Select TGZ and DEB as the packager on Linux
    set(CPACK_GENERATOR "DEB;TGZ")
    set(CPACK_SYSTEM_NAME "Linux")
    set(CPACK_SOURCE_IGNORE_FILES "/CVS/;/.svn/;/.swp$/;cscope.*;/.git/;${CMAKE_CURRENT_BINARY_DIR}/;/.bzr/;/.settings/;${DOT_FILES_BEGIN};${TILD_FILES}")

    # PACKAGING OPTIONS: DEB
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jose Fernandez <jose.fernandez.navarro@scilifelab.se>")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${TARGET_ARCH})
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libstdc++6, libglu1-mesa-dev, freeglut3-dev, mesa-common-dev, libboost-dev, libcairo-dev, flex, bison")

    if(32BIT_MODE)
        set(CPACK_SYSTEM_NAME "${CPACK_SYSTEM_NAME}32")
    else()
        set(CPACK_SYSTEM_NAME "${CPACK_SYSTEM_NAME}64")
    endif()

    # Qt libraries required by PrimeTV2
    # Note: unlike for Windows and OS X, we don't have a macro for deploying our
    # Qt libraries. Indeed, on Linux, the version of a file is appended to
    # its suffix, so unless making the macro unnecessarily cumbersome to
    # use, it's better to do things ourselves...

    foreach(QT_LIBRARY Core DBus Gui Help PrintSupport Widgets) #Qml Quick Sql Svg V8
        install(FILES ${QT_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}Qt${QT_VERSION_MAJOR}${QT_LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX}.${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}
                DESTINATION lib
                RENAME ${CMAKE_SHARED_LIBRARY_PREFIX}Qt${QT_VERSION_MAJOR}${QT_LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX}.${QT_VERSION_MAJOR})
    endforeach()

    foreach(QT_FILE icudata icui18n icuuc)
        install(FILES ${QT_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${QT_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}.49.1.2
                DESTINATION lib
                RENAME ${CMAKE_SHARED_LIBRARY_PREFIX}${QT_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}.49)
    endforeach()

    # Qt plugins required by PrimeTV2
    LINUX_DEPLOY_QT_PLUGIN(accessible qtaccessiblequick qtaccessiblewidgets)
    LINUX_DEPLOY_QT_PLUGIN(imageformats qgif qico qjpeg qtiff qsvg) #qmng  qtga  qwbmp)
    LINUX_DEPLOY_QT_PLUGIN(platforms qlinuxfb qminimal qxcb)
    LINUX_DEPLOY_QT_PLUGIN(printsupport cupsprintersupport)

    install(FILES ${CMAKE_SOURCE_DIR}/cmake/linux/qt.conf DESTINATION bin)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
    #install(FILES ${QT_LOCALES} DESTINATION share/${PROJECT_NAME}/translations)
    install(FILES ${PROJECT_SOURCE_DIR}/resources/images/header.png 
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/primetv 
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
            
    # Shell script to run PrimeTV2

    set(ShellFile ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.sh)

    configure_file(${CMAKE_SOURCE_DIR}/cmake/linux/application.sh.in ${ShellFile})

    install(FILES ${ShellFile} RENAME ${PROJECT_NAME}
            DESTINATION .
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

endif()

if(APPLE)

  set(InfoPlistFile ${CMAKE_BINARY_DIR}/Info.plist)

  execute_process(COMMAND date "+%Y"
                  OUTPUT_VARIABLE YEAR
                  OUTPUT_STRIP_TRAILING_WHITESPACE)

  configure_file(${CMAKE_SOURCE_DIR}/cmake/mac/Info.plist.in ${InfoPlistFile})

  set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${InfoPlistFile})

  # MACOSX sets
  #set(MACOSX_BUNDLE_ICON_FILE "${CMAKE_SOURCE_DIR}/assets/images/stVi.icns")
  set(MACOSX_BUNDLE_INFO_STRING "PrimeTV2 : Phylogenetic trees viewer")
  set(MACOSX_BUNDLE_GUI_IDENTIFIER "name.elemc.${PROJECT_NAME}")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_VERSION "${PROJECT_VERSION}")
  set(MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")

  set(CMAKE_OSX_ARCHITECTURES "ppc;i386")
  set(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")

  set(BUNDLE_DIR ${OS_X_PROJECT_BINARY_DIR})
  set(CMAKE_INSTALL_PREFIX ${BUNDLE_DIR}/Contents)
        
  add_custom_command (TARGET ${PROJECT_NAME}
      COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/Resources/translations
      #COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/resources/icons/icon.png ${CMAKE_INSTALL_PREFIX}/Resources/
      #COMMAND install -m 644 ${QT_LOCALES} ${CMAKE_INSTALL_PREFIX}/Resources/translations/
      COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/resources/images/header.png ${CMAKE_INSTALL_PREFIX}/Resources/ 
      COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/cmake/mac/qt.conf ${CMAKE_INSTALL_PREFIX}/Resources/
      #COMMAND ${MACDEPLOYQT} ${BUNDLE_DIR}
      )

  set(DMG_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

  #### Plugins ####
  OS_X_CLEAN_UP_FILE_WITH_QT_DEPENDENCIES(${OS_X_PROJECT_BINARY_DIR}/Contents/MacOS ${PROJECT_NAME}
                                            QtCore QtGui QtHelp QtPrintSupport
                                            QtWidgets QtSvg) #QtQml QtQuick QtSql QtV8

  # Deploy Qt dependencies
  OS_X_DEPLOY_QT_LIBRARIES(QtCore QtGui QtHelp
                           QtPrintSupport QtSvg
                           QtWidgets) # QtQml QtQuick QtSql QtV8

  # Deploy some plugins
  OS_X_DEPLOY_QT_PLUGIN(imageformats qgif qico qjpeg qsvg qtiff) #qmng  qtga  qwbmp
  OS_X_DEPLOY_QT_PLUGIN(platforms qcocoa)
  OS_X_DEPLOY_QT_PLUGIN(printsupport cocoaprintersupport)
  
  add_custom_target(dmg
      COMMAND mkdir -p ${DMG_DIR}
      COMMAND cp -R ${BUNDLE_DIR} ${DMG_DIR}/
      COMMAND ln -sf /Applications ${DMG_DIR}
      COMMAND cp ${CMAKE_SOURCE_DIR}/resources/images/header.png .
      #COMMAND cp ${CMAKE_SOURCE_DIR}/assets/images/stVi.icns .VolumeIcon.icns
      COMMAND hdiutil create ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}.dmg -srcfolder ${DMG_DIR}
      DEPENDS ${PROJECT_NAME}
      COMMENT "Create DMG image"
  )

endif(APPLE)

if(WIN32)

    set(CPACK_GENERATOR "NSIS") # Distribution packaging formats ("make package")
    set(CPACK_NSIS_MODIFY_PATH "ON")
    set(CPACK_NSIS_CONTACT "Jose Fernandez <jose.fernandez.navarro@scilifelab.se>")
    #set(CPACK_PACKAGE_ICON "")
    #set(CPACK_NSIS_MUI_ICON "")
    #set(CPACK_NSIS_PACKAGE_NAME "")

    # Qt libraries required by PrimeTV2

    WINDOWS_DEPLOY_QT_LIBRARIES(Qt5Core Qt5Gui Qt5Help 
                                Qt5PrintSupport Qt5Svg Qt5Widgets) # Qt5Multimedia Qt5MultimediaWidgets  Qt5Qml Qt5Quick Qt5Sql Qt5V8
    #WINDOWS_DEPLOY_QT_LIBRARIES(icudt49 icuin49 icuuc49)

    # Qt plugins required by PrimeTV2

    WINDOWS_DEPLOY_QT_PLUGIN(accessible qtaccessiblequick qtaccessiblewidgets)
    WINDOWS_DEPLOY_QT_PLUGIN(imageformats qgif qico qjpeg qsvg qtiff) #qmng  qtga  qwbmp

    foreach(QT_PLUGIN_FILE qminimal qwindows)
        install(FILES ${QT_PLUGINS_DIR}/platforms/${CMAKE_SHARED_LIBRARY_PREFIX}${QT_PLUGIN_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}
                DESTINATION bin/platforms)
        # Note: yes, the platforms plugin must be installed in the bin folder
        # and not the plugins folder...
    endforeach()

    WINDOWS_DEPLOY_QT_PLUGIN(printsupport windowsprintersupport)
    WINDOWS_DEPLOY_QT_PLUGIN(sqldrivers qsqlite)

    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
    #install(FILES ${QT_LOCALES} DESTINATION translations)
    install(FILES ${PROJECT_SOURCE_DIR}/resources/images/header.png 
            DESTINATION /share/primetv)
            
endif(WIN32)

include(CPack)

