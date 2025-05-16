set(CMAKE_CXX_STANDARD 20)

project(platformer_nav)

add_subdirectory(thirdparty/box2d)
add_subdirectory(thirdparty/raylib)

add_executable(platformer_nav
	src/main.cc
	src/agent.cc
	src/physics.cc
	src/interface.cc
	src/tilemap.cc
	src/flow_map.cc
)

target_link_libraries(platformer_nav box2d raylib)
