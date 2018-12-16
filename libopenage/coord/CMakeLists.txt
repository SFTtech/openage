# Copyright 2018-2018 the openage authors. See copying.md for legal info.

add_sources(libopenage
	chunk.cpp
	coord_test.cpp
	coordmanager.cpp
	declarations.cpp
	phys.cpp
	pixel.cpp
	term.cpp
	tile.cpp
)

target_include_directories(libopenage
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}
)
