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
#location of some files needed by primetv
set(HEADER_LOCATION "${CMAKE_INSTALL_PREFIX}/share/primetv/header.png")
add_definitions(-DHAS_HEADER="${HEADER_LOCATION}")

# Organize projects into folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(INC_MAIN
    Mainops.h
    Parameters.h
)

set(SRC_MAIN
    Mainops.cpp
    Parameters.cpp
)

set(INC_DRAW
    draw/Color.h
    draw/Colours.h
    draw/DrawTreeCairo.h
)

set(SRC_DRAW
    draw/Color.cpp
    draw/Colours.cpp
    draw/DrawTreeCairo.cpp
)

set(INC_GUI
    gui/Canvas.h
    gui/GraphicView.h
    gui/Windows.h
)

set(SRC_GUI
    gui/Canvas.cpp
    gui/GraphicView.cpp
    gui/Windows.cpp
)

set(INC_LAYOUT
    layout/Edge.h
    layout/Layouttrees.h
)

set(SRC_LAYOUT
    layout/Edge.cpp
    layout/Layoutrees.cpp
)

set(INC_LGT
    lgt/Phyltr.h
)

set(SRC_LGT
    lgt/Phyltr.cpp
)

set(INC_PARSER
    parser/NHXannotation.h
    parser/NHXnode.h
    parser/NHXtree.h
)

set(SRC_PARSER
    parser/NHXannotation.c
    parser/NHXnode.c
    parser/NHXtree.c
)

set(INC_RECONCILATION
    reconcilation/BeepVector.hh
    reconcilation/GammaMapEx.h
    reconcilation/LambdaMapEx.h
    reconcilation/SetOfNodesEx.hh
    reconcilation/StrStrMap.hh
)

set(SRC_RECONCILATION
    reconcilation/StrStrMap.cc
)

set(INC_TREE
    tree/Node.hh
    tree/Tree.hh
    tree/Treeextended.h
    tree/TreeIO.hh
    tree/TreeIOTraits.hh
)

set(SRC_TREE
    tree/Node.cc
    tree/Tree.cc
    tree/Treeextended.cpp
    tree/TreeIO.cc
    tree/TreeIOTraits.cc
)

set(INC_UTILS
    utils/AnError.hh
    utils/ConfigFile.h
)

set(SRC_UTILS
    utils/AnError.cc
    utils/ConfigFile.cpp
)

set(INCLUDES
    ${INC_MAIN}
    ${INC_DRAW}
    ${INC_GUI}
    ${INC_LAYOUT}
    ${INC_LGT}
    ${INC_PARSER}
    ${INC_RECONCILATION}
    ${INC_TREE}
    ${INC_UTILS}
)

set(SOURCES
    ${SRC_MAIN}
    ${SRC_DRAW}
    ${SRC_GUI}
    ${SRC_LAYOUT}
    ${SRC_LGT}
    ${SRC_PARSER}
    ${SRC_RECONCILATION}
    ${SRC_TREE}
    ${SRC_UTILS}
)

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
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS system program_options)  
include_directories(${Boost_INCLUDE_DIRS})
#find bison flex and cairo
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(Cairo REQUIRED)
include_directories(${CAIRO_INCLUDE_DIR})

####PACKAGES##################################################

###DEFINITIONS###################################################

add_definitions(-DPROJECT_VERSION=\"${PROJECT_VERSION}\")

###DEFINITIONS###################################################

bison_target(MyParser "${PROJECT_SOURCE_DIR}/src/parser/NHXparse.y"
                      "${PROJECT_SOURCE_DIR}/src/parser/NHXparse.c"
                      COMPILE_FLAGS "-d -y -p yytree_" ) 
flex_target(MyScanner "${PROJECT_SOURCE_DIR}/src/parser/NHX.l"
                      "${PROJECT_SOURCE_DIR}/src/parser/lexer.c"
                      COMPILE_FLAGS "-Pyytree_" )
add_flex_bison_dependency(MyScanner MyParser)

#QT5 DIRECTIVES
set(FORMS "${PROJECT_SOURCE_DIR}/resources/ui/primetv.ui" 
          "${PROJECT_SOURCE_DIR}/resources/ui/parameters.ui")
          
set(RESOURCES "${PROJECT_SOURCE_DIR}/resources/application.qrc")

qt5_wrap_ui(QT_FORMS ${FORMS})
qt5_add_resources(QT_RESOURCES ${RESOURCES})

#set up tree parsers
set(PARSER_SOURCES "${SRC_PARSER}"
                   "${BISON_MyParser_OUTPUTS}" 
                   "${FLEX_MyScanner_OUTPUTS}")
		  
foreach(i ${PARSER_SOURCES})
 set_source_files_properties(${i} PROPERTIES LANGUAGE C)
endforeach(i)

add_library(primetvlib STATIC ${PARSER_SOURCES} ${SOURCES} ${QT_FORMS})

if(WIN32)
  add_executable(${PROJECT_NAME} WIN32 primetv.cpp ${QT_RESOURCES})
elseif(APPLE)
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE primetv.cpp ${QT_RESOURCES})
else()
  add_executable(${PROJECT_NAME} primetv.cpp ${QT_RESOURCES})
endif()

qt5_use_modules(${PROJECT_NAME} Core Gui Widgets WebKitWidgets LinguistTools) 
target_link_libraries(${PROJECT_NAME} primetvlib ${CAIRO_LIBRARIES} ${Boost_LIBRARIES}) 


### UNIT TESTS ################################################################

enable_testing()
add_subdirectory(test)

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
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libstdc++6, libboost-dev, libcairo-dev, libqt5-dev, flex, bison")

    if(32BIT_MODE)
        set(CPACK_SYSTEM_NAME "${CPACK_SYSTEM_NAME}32")
    else()
        set(CPACK_SYSTEM_NAME "${CPACK_SYSTEM_NAME}64")
    endif()

    foreach(QT_LIBRARY Core Gui Widgets WebKitWidgets LinguistTools)
        install(FILES ${QT_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}Qt${QT_VERSION_MAJOR}${QT_LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX}.${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}
                DESTINATION lib
                RENAME ${CMAKE_SHARED_LIBRARY_PREFIX}Qt${QT_VERSION_MAJOR}${QT_LIBRARY}${CMAKE_SHARED_LIBRARY_SUFFIX}.${QT_VERSION_MAJOR})
    endforeach()

    set(ICU_VERSION "51.1") #qt5.1.1 icu is 51 for lower versions of qt5 icu is 49

    #qt needs ICU libaries in Linux
    foreach(QT_FILE icudata icui18n icuuc)
        install(FILES ${QT_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${QT_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}.${ICU_VERSION}
                DESTINATION lib
                RENAME ${CMAKE_SHARED_LIBRARY_PREFIX}${QT_FILE}${CMAKE_SHARED_LIBRARY_SUFFIX}.5)
    endforeach()

    # Qt plugins required by PrimeTV2
    LINUX_DEPLOY_QT_PLUGIN(accessible qtaccessiblequick qtaccessiblewidgets)
    LINUX_DEPLOY_QT_PLUGIN(imageformats qgif qico qjpeg qtiff qsvg) #qmng  qtga  qwbmp)
    LINUX_DEPLOY_QT_PLUGIN(platforms qlinuxfb qminimal qxcb)
    LINUX_DEPLOY_QT_PLUGIN(printsupport cupsprintersupport)

    install(FILES ${CMAKE_SOURCE_DIR}/cmake/linux/qt.conf DESTINATION bin)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
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

  #set project name of binary dir for OSX
  set(OS_X_PROJECT_BINARY_DIR "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.app")
  set(CMAKE_INSTALL_PREFIX ${OS_X_PROJECT_BINARY_DIR}/Contents)
  set(DMG_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
  #configure and install Info.plist file
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

  #find macdeoployqt
  find_program(MACDEPLOYQT NAMES macdeployqt macdeployqt-mac PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH)

  add_custom_command (TARGET ${PROJECT_NAME}
      COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/Resources/translations
      #COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/resources/icons/icon.png ${CMAKE_INSTALL_PREFIX}/Resources/
      COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/resources/images/header.png ${CMAKE_INSTALL_PREFIX}/Resources/ 
      COMMAND install -m 644 ${CMAKE_SOURCE_DIR}/cmake/mac/qt.conf ${CMAKE_INSTALL_PREFIX}/Resources/
      COMMAND ${MACDEPLOYQT} ${OS_X_PROJECT_BINARY_DIR}
      )
  
  add_custom_target(dmg
      COMMAND mkdir -p ${DMG_DIR}
      COMMAND cp -R ${BUNDLE_DIR} ${DMG_DIR}/
      COMMAND ln -sf /Applications ${DMG_DIR}
      COMMAND cp ${CMAKE_SOURCE_DIR}/resources/images/header.png .
      #COMMAND cp ${CMAKE_SOURCE_DIR}/resources/icons/icon.png  .VolumeIcon.icns
      COMMAND hdiutil create ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}.dmg -srcfolder ${DMG_DIR}
      DEPENDS ${PROJECT_NAME}
      COMMENT "Create DMG image"
  )

endif(APPLE)

include(CPack)

