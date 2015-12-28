set(LIGHTMAN_DISP_SOURCE
  lmUtils.cpp
  lmViewport.cpp
)

set(LIGHTMAN_DISP_HEADER
  lmUtils.hpp
  lmViewport.hpp
)

add_library(LightmanDisp
  ${LIGHTMAN_DISP_SOURCE}
  ${LIGHTMAN_DISP_HEADER}
)

target_link_libraries(LightmanDisp
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
)
