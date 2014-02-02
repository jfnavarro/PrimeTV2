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
    layout/Layoutrees.h
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
    reconcilation/BeepVector.h
    reconcilation/GammaMapEx.h
    reconcilation/LambdaMapEx.h
    reconcilation/SetOfNodesEx.h
    reconcilation/StrStrMap.h
)

set(SRC_RECONCILATION
    reconcilation/SetOfNodesEx.h
    reconcilation/BeepVector.h
    reconcilation/StrStrMap.cpp
    reconcilation/LambdaMapEx.cpp
    reconcilation/GammaMapEx.cpp
)

set(INC_TREE
    tree/Node.h
    tree/Tree.h
    tree/Treeextended.h
    tree/TreeIO.h
    tree/TreeIOTraits.h
)

set(SRC_TREE
    tree/Node.cpp
    tree/Tree.cpp
    tree/Treeextended.cpp
    tree/TreeIO.cpp
    tree/TreeIOTraits.cpp
)

set(INC_UTILS
    utils/AnError.h
    utils/ConfigFile.h
)

set(SRC_UTILS
    utils/AnError.cpp
    utils/ConfigFile.cpp
)

set(INCLUDES
    ${INC_MAIN}
    ${INC_DRAW}
    ${INC_GUI}
    ${INC_LAYOUT}
    ${INC_LGT}
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
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_RUNTIME ON)
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
set(FORMS
          "${PROJECT_SOURCE_DIR}/resources/ui/primetv.ui"
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

add_library(primetvlib STATIC ${PARSER_SOURCES} ${SOURCES} ${INCLUDES} ${QT_FORMS})

if(WIN32)
  add_executable(${PROJECT_NAME} WIN32 primetv.cpp ${QT_RESOURCES})
elseif(APPLE)
  add_executable(${PROJECT_NAME} primetv.cpp ${QT_RESOURCES}) 
else()
  add_executable(${PROJECT_NAME} primetv.cpp ${QT_RESOURCES})
endif()

qt5_use_modules(${PROJECT_NAME} Core Gui Widgets WebKitWidgets LinguistTools) 
target_link_libraries(${PROJECT_NAME} primetvlib ${CAIRO_LIBRARIES} ${Boost_LIBRARIES})

if(APPLE)
  set_property(TARGET ${PROJECT_NAME} PROPERTY LINK_SEARCH_END_STATIC ON)
endif(APPLE)

### UNIT TESTS ################################################################

enable_testing()
add_subdirectory(test)

######################################INSTALLATION##############################

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

    #install(FILES ${CMAKE_SOURCE_DIR}/cmake/linux/qt.conf DESTINATION bin)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
    install(FILES ${PROJECT_SOURCE_DIR}/resources/images/header.png 
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/primetv 
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

endif()

if(APPLE)

   set(CMAKE_OSX_ARCHITECTURES "ppc;i386")
   set(CMAKE_CXX_FLAGS "--sysroot ${CMAKE_OSX_SYSROOT} ${CMAKE_CXX_FLAGS}")

   #install(FILES ${CMAKE_SOURCE_DIR}/cmake/linux/qt.conf DESTINATION bin)
   install(TARGETS ${PROJECT_NAME} DESTINATION bin)
   install(FILES ${PROJECT_SOURCE_DIR}/resources/images/header.png
            DESTINATION ${CMAKE_INSTALL_PREFIX}/share/primetv
            PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

endif(APPLE)

include(CPack)

