include_directories(
    glm
    core
    display
    simulation
    application
    ${GLEW_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIR}
    ${ILMBASE_INCLUDE_DIR}
    ${OPENEXR_INCLUDE_DIR}
    ${JANSSON_INCLUDE_DIR}
)

# libgen windows port
IF (WIN32)
  include_directories(win32)
  add_subdirectory(win32)
ENDIF(WIN32)

add_subdirectory(core)
add_subdirectory(display)
add_subdirectory(simulation)
add_subdirectory(application)

# Build Arnold Support
if (LIGHTMAN_BUILD_ARNOLD_SUPPORT)
  find_package( Arnold REQUIRED )
  include_directories(${ARNOLD_INCLUDE_DIRS})
  link_directories(${ARNOLD_LIBRARY_DIRS})
  add_subdirectory(arnold)
  add_definitions( -DLIGHTMAN_ARNOLD_SUPPORT )
endif()

# Build samples
if(LIGHTMAN_BUILD_SAMPLES)
  add_subdirectory(sample)
endif(LIGHTMAN_BUILD_SAMPLES)

# Install glm headers
install(DIRECTORY glm
        DESTINATION /usr/local/include/Lightman)
