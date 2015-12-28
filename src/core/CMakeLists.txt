# core source files
set(LIGHTMAN_CORE_SOURCE
  lmPixel.cpp
  lmLayer.cpp
  lmCompositor.cpp
  lmToneMapper.cpp
  lmAnimation.cpp
  lmWorkspace.cpp
)

set(LIGHTMAN_CORE_HEADER
  LightmanCore.h
  lmPixel.hpp
  lmLayer.hpp
  lmCompositor.hpp
  lmToneMapper.hpp
  lmAnimation.hpp
  lmWorkspace.hpp
  lmConsole.hpp
  lmArnold.hpp
)

# build static library
add_library(LightmanCore STATIC
  ${LIGHTMAN_CORE_SOURCE}
  ${LIGHTMAN_CORE_HEADER}
)

target_link_libraries(LightmanCore
  ${ARNOLD_LIBRARIES}
  ${ILMBASE_LIBRARIES}
  ${OPENEXR_LIBRARIES}
  ${JANSSON_LIBRARIES}
)

install(TARGETS LightmanCore DESTINATION /usr/local/lib/)

# build shared library
if(LIGHTMAN_BUILD_SHARED)

  add_library(LightmanCoreShared SHARED
    ${LIGHTMAN_CORE_SOURCE}
    ${LIGHTMAN_CORE_HEADER}
  )

  target_link_libraries(LightmanCoreShared
    ${ARNOLD_LIBRARIES}
    ${ILMBASE_LIBRARIES}
    ${OPENEXR_LIBRARIES}
    ${JANSSON_LIBRARIES}
  )

  set_target_properties(LightmanCoreShared PROPERTIES OUTPUT_NAME LightmanCore)
  install(TARGETS LightmanCoreShared DESTINATION /usr/local/lib/)

endif(LIGHTMAN_BUILD_SHARED)

# install headers
install(FILES ${LIGHTMAN_CORE_HEADER}
        DESTINATION /usr/local/include/Lightman/)
