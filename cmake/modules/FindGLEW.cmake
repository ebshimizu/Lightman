# Look for local installation of GLEW
# If a local installation is found, the following variables will be defined:
#
# GLEW_FOUND
# GLEW_INCLUDE_DIR
# GLEW_LIBRARIES
#
# Please note that the find module is mostly written for windows, to use
# GLEW on unix systems, consider using cmake with pkg-config.
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

IF(WIN32)
	FIND_PATH(GLEW_INCLUDE_PATH GL/glew.h
		$ENV{PROGRAMFILES}/GLEW/include
		DOC "The directory where GL/glew.h resides")
	FIND_LIBRARY(GLEW_LIBRARIES
		NAMES glew GLEW glew32 glew32s
		PATHS
		$ENV{PROGRAMFILES}/GLEW/lib
		$ENV{PROGRAMFILES}/GLEW/lib/Release/Win32
		DOC "The GLEW library")
ELSE(WIN32)
	FIND_PATH( GLEW_INCLUDE_PATH GL/glew.h
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		DOC "The directory where GL/glew.h resides")
	FIND_LIBRARY( GLEW_LIBRARIES
		NAMES GLEW glew
		PATHS
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		DOC "The GLEW library")
ENDIF (WIN32)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLEW DEFAULT_MSG
  GLEW_INCLUDE_PATH
  GLEW_LIBRARIES
)
