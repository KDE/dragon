project(VideoPlayer)

set( CMAKE_MODULE_PATH    ${CMAKE_MODULE_PATH}
                          ${CMAKE_CURRENT_SOURCE_DIR}/cmake )


find_package(KDE4 REQUIRED)
find_package(Xine REQUIRED)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} )
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/src/app
    ${KDE4_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    )

add_subdirectory( doc/dragonplayer )
add_subdirectory( src/app )
add_subdirectory( misc )
add_subdirectory( cmake )
