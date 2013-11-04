###############################################################################
# Unit Test CMake                                                             #
###############################################################################

use_qt5lib(Qt5Test)

include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/src ${CMAKE_BINARY_DIR})

set(TARGET_NAME primetv_test)

# Define source files
set(SRC_${TARGET_NAME}
    GeneralTests.cpp
    TestSuite.cpp
    Breadthfirsttopdown.cpp
)

# add executable
if(WIN32)
    add_executable(${TARGET_NAME} WIN32 unit_main.cpp ${SRC_${TARGET_NAME}})
    target_link_libraries(${TARGET_NAME} ${Qt5Core_QTMAIN_LIBRARIES}) #Note: allows compile windows application (ie. not console)
else()
    add_executable(${TARGET_NAME} unit_main.cpp ${SRC_${TARGET_NAME}})
endif()

qt5_use_modules(${TARGET_NAME} Test Widgets Core Gui PrintSupport)
target_link_libraries(${TARGET_NAME} primetvlib ${CAIRO_LIBRARIES} ${Boost_LIBRARIES})

add_test(general ${TARGET_NAME})
add_custom_target(test ${TARGET_NAME})
