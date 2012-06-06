project(DragonPlayer)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

include_directories(${KDE4_INCLUDES})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_BINARY_DIR}
    )

include(CheckIncludeFiles)
check_include_files(unistd.h HAVE_UNISTD_H)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory( src/app )
add_subdirectory( misc )
add_subdirectory( doc )
