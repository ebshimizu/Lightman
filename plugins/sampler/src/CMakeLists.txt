cmake_minimum_required(VERSION 2.8)
project(sampler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -m64 -O3 -pthread -funroll-loops -Wno-narrowing")

# modules
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake/modules/")

# Lumiverse
find_package( Lumiverse REQUIRED)

# include directories
include_directories( ${LUMIVERSE_INCLUDE_DIR} )

# executables
add_executable(sampler main.cpp)

# linking
target_link_libraries( sampler ${LUMIVERSE_LIBRARIES} )

# install time
install(TARGETS sampler DESTINATION ${PROJECT_SOURCE_DIR}/../)
