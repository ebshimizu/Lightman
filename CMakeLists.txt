cmake_minimum_required(VERSION 3.0)
project(Lightman)

# Configuration
option(LIGHTMAN_BUILD_SHARED              "Build shared libraries"        ON)
option(LIGHTMAN_BUILD_SAMPLES             "Build samples"                 ON)
option(LIGHTMAN_BUILD_DOCUMENTATION       "Build documentation"           OFF)
option(LIGHTMAN_BUILD_ARNOLD_SUPPORT      "Build with Arnold support"     OFF)

# Check compiler
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
  set(CLANG_CXX_FLAGS "-std=c++11 -m64 -O3 -funroll-loops")
  set(CLANG_CXX_FLAGS "${CLANG_CXX_FLAGS} -Wno-narrowing")
  set(CLANG_CXX_FLAGS "${CLANG_CXX_FLAGS} -Wno-deprecated-register")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_CXX_FLAGS}")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  set(GCC_CXX_FLAGS "-std=gnu++11 -m64 -O3 -fopenmp -funroll-loops")
  set(GCC_CXX_FLAGS "${GCC_CXX_FLAGS} -Wno-narrowing")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CXX_FLAGS}")
endif(${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")

# Add modules
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake/modules/")

# Resolve rpath on OSX
if(APPLE)
  set(CMAKE_SKIP_BUILD_RPATH  false)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH false)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH true)
  list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
  if("${isSystemDir}" STREQUAL "-1")
     set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  endif("${isSystemDir}" STREQUAL "-1")
endif(APPLE)

# Dependencies
find_package( OpenGL REQUIRED ) # OpenGL

# Dependencies with pkg-config
IF (APPLE OR LINUX)
  find_package( PkgConfig REQUIRED )
  pkg_search_module( GLEW REQUIRED glew  )         # GLEW
  pkg_search_module( GLFW REQUIRED glfw3 )         # GLFW
  pkg_search_module( ILMBASE REQUIRED IlmBase )    # IlmBase
  pkg_search_module( OPENEXR REQUIRED OpenEXR )    # OpenEXR

  # Jansson
  pkg_search_module( JANSSON REQUIRED jansson )
  if (NOT jansson_FOUND)
    add_subdirectory(external/jansson)
  endif()

ELSEIF (WIN32)

  find_package(GLEW REQUIRED)
  find_package(GLFW REQUIRED)
  find_package(ILMBASE REQUIRED)
  find_package(OPENEXR REQUIRED)
  find_package(JANSSON REQUIRED )

ENDIF()

# Lightman
add_subdirectory(src)
