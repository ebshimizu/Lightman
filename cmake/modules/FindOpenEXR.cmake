# Look for local installation of OpenEXR library (version 2.2)
# If a local installation is found, the following variables will be defined:
#
# OPENEXR_FOUND
# OPENEXR_INCLUDE_DIR
# OPENEXR_LIBRARIES
#
# Please note that the find module is mostly written for windows, to use
# OpenEXR on unix systems, consider using cmake with pkg-config.
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

# ILMBASE required
IF(NOT DEFINED ILMBASE_LOCATION)
  MESSAGE(FATAL_ERROR "ilmbase library not found")
  MESSAGE(FATAL_ERROR "ilmbase is required by openexr, please use find_package
  to find ILMBASE before OpenEXR")
ENDIF()

IF(NOT DEFINED OPENEXR_LOCATION)
  IF (UNIX)
      SET(OPENEXR_LOCATION /usr/local)
  ELSEIF(WIN32)
    # Note: This assumes that OpenEXR was installed under the default
    # location in the cmake configurations shipped in version 2.2
    SET(OPENEXR_LOCATION "$ENV{PROGRAMFILES}/openexr" )
  ENDIF()
ENDIF()

SET(OPENEXR_LIB_PATHS
  "${OPENEXR_LOCATION}/lib"
  "${OPENEXR_LOCATION}/lib/Release"
  "${OPENEXR_LOCATION}/lib/x64/Release"
  "$ENV{OPENEXR_LOCATION}/lib"
  "$ENV{OPENEXR_LOCATION}/lib/Release"
  "$ENV{OPENEXR_LOCATION}/lib/x64/Release"
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

SET(OPENEXR_INCLUDE_PATHS
  "${OPENEXR_LOCATION}/include"
  "${OPENEXR_LOCATION}/include/OpenEXR/"
  "$ENV{OPENEXR_LOCATION}/include"
  "$ENV{OPENEXR_LOCATION}/include/OpenEXR/"
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

FIND_PATH(OPENEXR_INCLUDE_DIR /ImfXdr.h PATHS ${OPENEXR_INCLUDE_PATHS})
IF(${OPENEXR_INCLUDE_DIR} STREQUAL "OPENEXR_INCLUDE_DIR-NOTFOUND")
  MESSAGE(FATAL_ERROR "OpenEXR header files not found")
  MESSAGE(FATAL_ERROR "OPENEXR_LOCATION: ${OPENEXR_LOCATION}")
ENDIF()

FIND_LIBRARY(ILMBASE_HALF_LIB Half PATHS ${ILMBASE_LIB_PATHS})
IF(${ILMBASE_HALF_LIB} STREQUAL "ILMBASE_HALF_LIB-NOTFOUND")
  MESSAGE(FATAL_ERROR "ILMBase library (Half) not foun")
  MESSAGE(FATAL_ERROR "ILM_LIB_PATHS: ${ILMBASE_LIB_PATHS}")
ENDIF()

FIND_LIBRARY(OPENEXR_ILMIMF_LIB IlmImf-2_2 PATHS ${OPENEXR_LIB_PATHS})
IF(${OPENEXR_ILMIMF_LIB} STREQUAL "OPENEXR_ILMIMF_LIB-NOTFOUND")
  MESSAGE(FATAL_ERROR "OpenEXR library (IlmImf) not foun")
  MESSAGE(FATAL_ERROR "OPENEXR_LOCATION: ${OPENEXR_LOCATION}")
ENDIF()

SET(OPENEXR_LIBRARIES
  ${ILMBASE_HALF_LIB}
  ${ILMBASE_LIBRARIES}
  ${OPENEXR_ILMIMF_LIB}
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OPENEXR DEFAULT_MSG
  OPENEXR_INCLUDE_DIR
  OPENEXR_LIBRARIES
)
