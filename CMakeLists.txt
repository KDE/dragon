project(VideoPlayer)

find_package(KDE4 REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake )

#include(KDE4Defaults)
#include(MacroLibrary)
macro_optional_find_package(Xine)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DHAVE_CONFIG_H=1)
include_directories (${CMAKE_SOURCE_DIR} 
    ${CMAKE_BINARY_DIR} 
    ${KDE4_INCLUDES}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/app 
    ${XINE_INCLUDE_DIR} )

add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS) 
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w") #we're still worrying about all the errors

add_subdirectory( src/app )

