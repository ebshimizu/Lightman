# simulation source files
set(LIGHTMAN_SIM_SOURCE
  lmSimulation.cpp
)

set(LIGHTMAN_SIM_HEADER
  LightmanSimulation.h
  lmSimulation.hpp
)

# build static library
add_library(LightmanSim STATIC
  ${LIGHTMAN_SIM_SOURCE}
  ${LIGHTMAN_SIM_HEADER}
)

target_link_libraries(LightmanSim
  LightmanCore
  LightmanDisp
  ${GLEW_LIBRARIES}
  ${GLFW_LIBRARIES}
)

install(TARGETS LightmanSim DESTINATION /usr/local/lib/)

# build shared library
if(LIGHTMAN_BUILD_SHARED)

  add_library(LightmanSimShared SHARED
    ${LIGHTMAN_SIM_SOURCE}
    ${LIGHTMAN_SIM_HEADER}
  )

  target_link_libraries( LightmanSimShared
    LightmanDisp
    LightmanCoreShared
    ${GLFW_LIBRARIES}
    ${GLFW_LIBRARIES}
  )

  set_target_properties(LightmanSimShared PROPERTIES OUTPUT_NAME LightmanSim)
  install(TARGETS LightmanSimShared DESTINATION /usr/local/lib/)

endif(LIGHTMAN_BUILD_SHARED)

# install headers
install(FILES ${LIGHTMAN_SIM_HEADER}
        DESTINATION /usr/local/include/Lightman)
