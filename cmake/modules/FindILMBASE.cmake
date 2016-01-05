# Look for local installation of ILMBASE library (version 2.2)
# If a local installation is found, the following variables will be defined:
#
# ILMBASE_FOUND
# ILMBASE_INCLUDE_DIR
# ILMBASE_LIBRARIES
#
# Please note that the find module is mostly written for windows, to use
# ILMBase on unix systems, consider using cmake with pkg-config.
# Sky Gao, 2016
#
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

IF(NOT DEFINED ILMBASE_LOCATION)
  IF(UNIX)
    SET(ILMBASE_LOCATION /usr/local)
  ELSEIF(WIN32)
    # Note: This assumes that ILMBASE was installed under the default
    # location in the cmake configurations shipped in version 2.2
    SET(ILMBASE_LOCATION "$ENV{PROGRAMFILES}/ilmbase" )
  ENDIF()
ENDIF()

SET(ILMBASE_LIB_PATHS
  "${ILMBASE_LOCATION}/lib"
  "${ILMBASE_LOCATION}/lib/Release"
  "${ILMBASE_LOCATION}/lib/x64/Release"
  "$ENV{ILMBASE_LOCATION}/lib"
  "$ENV{ILMBASE_LOCATION}/lib/Release"
  "$ENV{ILMBASE_LOCATION}/lib/x64/Release"
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local/lib
  /usr/lib
  /sw/lib
  /opt/local/lib
  /opt/csw/lib
  /opt/lib
  /usr/freeware/lib64
  NO_DEFAULT_PATH
  NO_CMAKE_ENVIRONMENT_PATH
  NO_CMAKE_PATH
  NO_SYSTEM_ENVIRONMENT_PATH
  NO_CMAKE_SYSTEM_PATH
)

SET(ILMBASE_INCLUDE_PATHS
  "${ILMBASE_LOCATION}/include"
  "${ILMBASE_LOCATION}/include/OpenEXR/"
  "$ENV{ILMBASE_LOCATION}/include"
  "$ENV{ILMBASE_LOCATION}/include/OpenEXR/"
  /usr/local/include
  /usr/local/include/OpenEXR/
  /usr/include
  /usr/include/OpenEXR
  /opt/include
  /opt/local/include # Macports
  /usr/local/Cellar  # Homebrew
  /opt/csw/include   # Blastwave
  /sw/include        # Fink
)


FIND_PATH(ILMBASE_INCLUDE_DIR ImathMath.h PATHS ${ILMBASE_INCLUDE_PATHS})
IF(${ILMBASE_INCLUDE_DIR} STREQUAL "ILMBASE_INCLUDE_DIR-NOTFOUND")
  MESSAGE(FATAL_ERROR "ILMBase header files not found")
  MESSAGE(FATAL_ERROR "ILMBASE_LOCATION: ${ILMBASE_LOCATION}")
ENDIF()

FIND_LIBRARY(ILMBASE_IEX_LIB Iex-2_2 PATHS ${ILMBASE_LIB_PATHS})
IF(${ILMBASE_IEX_LIB} STREQUAL "ILMBASE_IEX_LIB-NOTFOUND")
  MESSAGE(FATAL_ERROR "ILMBase libraries (Iex) not foun")
  MESSAGE(FATAL_ERROR "ILMBASE_LOCATION: ${ILMBASE_LOCATION}")
ENDIF()

FIND_LIBRARY(ILMBASE_ILMTHREAD_LIB IlmThread-2_2 PATHS ${ILMBASE_LIB_PATHS})
IF(${ILMBASE_ILMTHREAD_LIB} STREQUAL "ILMBASE_ILMTHREAD_LIB-NOTFOUND")
  MESSAGE(FATAL_ERROR "ILMBase libraries (IlmThread) not found")
  MESSAGE(FATAL_ERROR "ILMBASE_LOCATION: ${ILMBASE_LOCATION}")
ENDIF()

FIND_LIBRARY(ILMBASE_IMATH_LIB Imath-2_2 PATHS ${ILMBASE_LIB_PATHS})
IF(${ILMBASE_IMATH_LIB} STREQUAL "ILMBASE_IMATH_LIB-NOTFOUND")
  MESSAGE(FATAL_ERROR "ILMBase libraries (Imath) not found")
  MESSAGE(FATAL_ERROR "ILMBASE_LOCATION: ${ILMBASE_LOCATION}")
ENDIF()

SET(ILMBASE_LIBRARIES
  ${ILMBASE_IMATH_LIB}
  ${ILMBASE_ILMTHREAD_LIB}
  ${ILMBASE_IEX_LIB}
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ILMBASE DEFAULT_MSG
  ILMBASE_INCLUDE_DIR
  ILMBASE_LIBRARIES
)
