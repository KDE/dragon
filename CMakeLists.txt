project(VideoPlayer)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )

find_package(KDE4 REQUIRED)
find_package(Xine REQUIRED)

#include(KDE4Defaults)
#include(MacroLibrary)
#macro_optional_find_package(Xine)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} )
include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/src
    ${CMAKE_BINARY_DIR}/src/app
    ${KDE4_INCLUDES}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/app
    ${XINE_INCLUDE_DIR} )

message( STATUS, "    ${KDE4_KUTILS_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${XINE_LIBRARY}
    ${KDE4_KDE3SUPPORT_LIBS} " )

add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS) 
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w") #we're still worrying about all the errors

add_subdirectory( src/app )

