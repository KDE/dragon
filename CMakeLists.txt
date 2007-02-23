project(VideoPlayer)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )

find_package(KDE4 REQUIRED)

#include(KDE4Defaults)
#include(MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} )
include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/src
    ${CMAKE_BINARY_DIR}/src/app
    ${KDE4_INCLUDES}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/app
    )

add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS) 

add_subdirectory( src/app )
add_subdirectory( misc )
