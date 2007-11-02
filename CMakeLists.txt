project(VideoPlayer)

find_package(KDE4 REQUIRED)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} )
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/src/app
    ${KDE4_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    )

add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS) 

add_subdirectory( src/app )
add_subdirectory( misc )
