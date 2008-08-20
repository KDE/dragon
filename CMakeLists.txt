project(DragonPlayer)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    )
if(XINE_XCB_FOUND)
include_directories(
    ${XINE_INCLUDE_DIR}
    )
endif(XINE_XCB_FOUND)    

add_subdirectory( src/app )
add_subdirectory( misc )
