# applicatiopn source files
set(LIGHTMAN_APP_SOURCE
    lmApplication.cpp
)

set(LIGHTMAN_APP_HEADER
    LightmanApplication.h
    lmApplication.hpp
)

# build static library
add_library (LightmanApp STATIC
  ${LIGHTMAN_APP_SOURCE}
  ${LIGHTMAN_APP_HEADER}
)

target_link_libraries(LightmanApp
  LightmanCore
  LightmanDisp
  ${GLFW_LIBRARIES}
)

install(TARGETS LightmanApp DESTINATION /usr/local/lib/)

# build shared library
if(LIGHTMAN_BUILD_SHARED)

  add_library (LightmanAppShared SHARED
    ${LIGHTMAN_APP_SOURCE}
    ${LIGHTMAN_APP_HEADER}
  )

  target_link_libraries( LightmanAppShared
    LightmanCore
    LightmanDisp
    ${GLFW_LIBRARIES}
  )

  set_target_properties(LightmanAppShared PROPERTIES OUTPUT_NAME LightmanApp)
  install(TARGETS LightmanAppShared DESTINATION /usr/local/lib/)

endif(LIGHTMAN_BUILD_SHARED)

# install headers
install(FILES ${LIGHTMAN_APP_HEADER}
        DESTINATION /usr/local/include/Lightman)
