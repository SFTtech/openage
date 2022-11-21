add_sources(libopenage
	game.cpp
	player.cpp
	terrain.cpp
	terrain_chunk.cpp
	world.cpp
	universe.cpp
)

# TODO: remove once migration is done.
add_subdirectory(old/)
