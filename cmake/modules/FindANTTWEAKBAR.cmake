# Find AntTweakBar library and include paths
# This defines the following variables:
#
# ANTTWEAKBAR_FOUND             if AntTweakBar is found
# ANTTWEAKBAR_LIBRARY           Path to AntTweakBar library
# ANTTWEAKBAR_LIBRARIES         Path to AntTweakBar library
# ANTTWEAKBAR_INCLUDE_DIR       Path to AntTweakBar include directory
# ANTTWEAKBAR_INCLUDE_DIRS      Path to AntTweakBar include directory
# ANTTWEAKBAR_VERSION           Version of AntTweakBar

# AntTweakBar library 
find_library( ANTTWEAKBAR_LIBRARY AntTweakBar
    NAMES AntTweakBar
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
    DOC "AntTweakBar library"
)

# AntTweakBar headers 
find_path( ANTTWEAKBAR_INCLUDE_DIR AntTweakBar.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
    DOC "AntTweakBar include path"
)

# For sake of backward compatability
set(ANTTWEAKBAR_LIBRARIES ${ANTTWEAKBAR_LIBRARY})
set(ANTTWEAKBAR_INCLUDE_DIRS ${ANTTWEAKBAR_INCLUDE_DIR})

# Get version
file(STRINGS ${ANTTWEAKBAR_INCLUDE_DIR}/AntTweakBar.h header_lines
     REGEX "^#define TW_VERSION[ \t]+[0-9]+.*$" )
list(LENGTH header_lines num_lines)
if(num_lines EQUAL 1)
    foreach(_line ${header_lines})
        string(REGEX MATCH "[0-9]+" num ${_line})
        if(NOT DEFINED ANTTWEAKBAR_VERSION)
            set(ANTTWEAKBAR_VERSION ${num})
        else()
            set(ANTTWEAKBAR_VERSION "${ANTTWEAKBAR_VERSION}.${num}")
        endif()
    endforeach()
else()
    message(WARNING "Could not determine AntTweakBar version")
endif()

# Set standard arguments for find package
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AntTweakBar
    REQUIRED_VARS ANTTWEAKBAR_LIBRARY ANTTWEAKBAR_INCLUDE_DIR
    VERSION_VAR ANTTWEAKBAR_VERSION)
