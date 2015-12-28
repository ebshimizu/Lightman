# Find Arnold SKD library and include paths
# This finds the Arnold based on the path to
# the root directory of Arnold SDK defined in:
#
# ARNOLD_ROOT_DIR          Arnold SDK root directory
# 
# When set correctly, this defines the following variables:
#
# ARNOLD_FOUND             If Arnold SDK is found
# ARNOLD_VERSION           Version of Arnold#
# ARNOLD_LIBRARY           Path to ai library
# ARNOLD_LIBRARIES         Path to ai library
# ARNOLD_INCLUDE_DIR       Path to the include directory
# ARNOLD_INCLUDE_DIRS      Path to the include directory

# Arnold library 
find_library(ARNOLD_LIBRARY
    NAMES ai
    PATHS ${ARNOLD_ROOT_DIR}/lib
    DOC "Arnold library")

# Arnold headers 
find_path(ARNOLD_INCLUDE_DIR ai.h
    PATHS ${ARNOLD_ROOT_DIR}/include
    DOC "Arnold include path")

# For sake of backward compatability
set(ARNOLD_LIBRARIES ${ARNOLD_LIBRARY})
set(ARNOLD_INCLUDE_DIRS ${ARNOLD_INCLUDE_DIR})

# Get the arnold version from ai_version.h
file(STRINGS ${ARNOLD_INCLUDE_DIR}/ai_version.h _lines
     REGEX "^#define AI_VERSION_[A-Z_ 0-9\"]+$" )
list(LENGTH _lines _linenum)
if(_linenum EQUAL 4)
    foreach(_line ${_lines})
        string(REGEX MATCH "[0-9]+" _num ${_line})
        if(NOT DEFINED ARNOLD_VERSION)
            set(ARNOLD_VERSION ${_num})
        else()
            set(ARNOLD_VERSION "${ARNOLD_VERSION}.${_num}")
        endif()
    endforeach()
else()
    message(WARNING "Could not determine ARNOLD_VERSION")
endif()

# Set standard arguments for find package
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Arnold 
    REQUIRED_VARS ARNOLD_LIBRARY ARNOLD_INCLUDE_DIR
    VERSION_VAR ARNOLD_VERSION)
