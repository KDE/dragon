project(VideoPlayer)

set( CMAKE_MODULE_PATH    ${CMAKE_MODULE_PATH}
                          ${CMAKE_CURRENT_SOURCE_DIR}/cmake )


find_package(KDE4 REQUIRED)
find_package(Xine REQUIRED)
if(NOT XINE_XCB_FOUND )
    message( FATAL_ERROR "Xine and phonon-xine must be compiled with XCB support.")
endif(NOT XINE_XCB_FOUND )


add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS} )
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/src/app
    ${KDE4_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${XINE_INCLUDE_DIR}
    )

add_subdirectory( src/app )
add_subdirectory( misc )
add_subdirectory( cmake )
