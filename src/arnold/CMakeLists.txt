# Arnold root path - change if Arnold is used
set(ARNOLD_ROOT_DIR "${PROJECT_SOURCE_DIR}/../external/arnold")

# Arnold buffer driver plugin
add_library(AiBufferDriver STATIC aiBufferDriver.cpp)

# Build with arnold SDK
IF(WIN32)
    add_library(arnold STATIC IMPORTED)
    set_property(TARGET arnold PROPERTY IMPORTED_LOCATION ${ARNOLD_LIBRARIES})
    target_link_libraries(AiBufferDriver arnold)
ENDIF(WIN32)

IF(APPLE)
    add_library(arnold SHARED IMPORTED)
    set_property(TARGET arnold PROPERTY IMPORTED_LOCATION ${ARNOLD_LIBRARIES})
    target_link_libraries(AiBufferDriver arnold)
ENDIF(APPLE)

# Installation time
install(TARGETS AiBufferDriver DESTINATION ${ARNOLD_ROOT_DIR}/plugin)
