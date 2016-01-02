# Look for local installation of jansson
# If a local installation is found, the following variables will be defined:
#
# JANSSON_FOUND
# JANSSON_INCLUDE_DIR
# JANSSON_LIBRARY
#
# Sky Gao, 2016
# Licensed under the WTFPL, Version 2
#
# DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
#             Version 2, December 2004
#
# Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
#
# Everyone is permitted to copy and distribute verbatim or modified
# copies of this license document, and changing it is allowed as long
# as the name is changed.
#
#     DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
# TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
#
# 0. You just DO WHAT THE FUCK YOU WANT TO.
#

IF(UNIX)
  SET(JANSSON_INCLUDE_PATHS
    /usr/include
    /usr/local/include
    /opt/local/include
    /sw/include
  )

  SET(JANSSON_LIBRARY_PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
  )
ELSEIF(WIN32)
  # Note: modify this if you had jasson installed else where
  SET(JANSSON_INCLUDE_PATHS "$ENV{PROGRAMFILES}/jansson/include" )
  SET(JANSSON_LIBRARY_PATHS "$ENV{PROGRAMFILES}/jansson/lib" )
ENDIF()

FIND_PATH(JANSSON_INCLUDE_DIR jansson.h PATHS ${JANSSON_INCLUDE_PATHS})
IF(${JANSSON_INCLUDE_DIR} STREQUAL "JANSSON_INCLUDE_DIR-NOTFOUND")
  MESSAGE(FATAL_ERROR "jansson header not found")
  MESSAGE(FATAL_ERROR "JANSSON_INCLUDE_PATHS: ${JANSSON_LOCATION}")
ENDIF()

FIND_LIBRARY(JANSSON_LIBRARIES NAMES jansson_d PATHS ${JANSSON_LIBRARY_PATHS})
IF(${JANSSON_LIBRARIES} STREQUAL "JANSSON_LIBRARIES-NOTFOUND")
  MESSAGE(FATAL_ERROR "jansson library not found")
  MESSAGE(FATAL_ERROR "JANSSON_LIBRARY_PATHS: ${JANSSON_LOCATION}")
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(JANSSON DEFAULT_MSG
  JANSSON_LIBRARIES JANSSON_INCLUDE_DIR)
