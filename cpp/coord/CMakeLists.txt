add_sources(${PROJECT_NAME}
	camgame.cpp
	camhud.cpp
	chunk.cpp
	phys2.cpp
	phys3.cpp
	term.cpp
	tests.cpp
	tile.cpp
	tile3.cpp
	vec2.cpp
	vec2f.cpp
	vec3.cpp
	vec3f.cpp
	window.cpp
)

add_test_cpp(openage::coord::tests::coord "test functionality of the coord system")
