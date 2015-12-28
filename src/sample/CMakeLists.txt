# link libraries
if (LIGHTMAN_BUILD_SHARED)
link_libraries(
  LightmanCoreShared
  LightmanAppShared
  LightmanSimShared
)
else (LIGHTMAN_BUILD_SHARED)
link_libraries(
  LightmanCore
  LightmanApp
  LightmanSim
)
endif(LIGHTMAN_BUILD_SHARED)

# Core exmaple
add_executable(core core.cpp)
install(TARGETS core DESTINATION ${PROJECT_SOURCE_DIR}/../sample/)

# Simulation exmaple
add_executable(application application.cpp)
install(TARGETS application DESTINATION ${PROJECT_SOURCE_DIR}/../sample/)

# Simulation exmaple
add_executable(simulation simulation.cpp)
install(TARGETS simulation DESTINATION ${PROJECT_SOURCE_DIR}/../sample/)
