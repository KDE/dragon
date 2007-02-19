# - Try to find the Xine library
# Once done this will define
#
#  XINE_FOUND - system has the Xine library
#  XINE_INCLUDE_DIR - the Xine include directory
#  XINE_LIBRARY - The libraries needed to use Xine

if (XINE_INCLUDE_DIR AND XINE_LIBRARY)
  # Already in cache, be silent
  set(XINE_FIND_QUIETLY TRUE)
endif (XINE_INCLUDE_DIR AND XINE_LIBRARY)

FIND_PATH(XINE_INCLUDE_DIR xine.h
 /usr/include/
 /usr/local/include/
)

FIND_LIBRARY(XINE_LIBRARY NAMES xine
 PATHS
 /usr/lib
 /usr/local/lib
)

if (XINE_INCLUDE_DIR AND XINE_LIBRARY)
   set(XINE_FOUND TRUE)
endif (XINE_INCLUDE_DIR AND XINE_LIBRARY)

if (XINE_FOUND)
   if (NOT XINE_FIND_QUIETLY)
      message(STATUS "Found Xine library: ${XINE_LIBRARY}")
   endif (NOT XINE_FIND_QUIETLY)
else (XINE_FOUND)
   if (XINE_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find Xine library")
   endif (XINE_FIND_REQUIRED)
endif (XINE_FOUND)

MARK_AS_ADVANCED(XINE_INCLUDE_DIR XINE_LIBRARY)
