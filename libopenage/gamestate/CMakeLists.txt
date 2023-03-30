add_sources(libopenage
    entity_factory.cpp
	game_entity.cpp
    game_state.cpp
	game.cpp
	player.cpp
	terrain_chunk.cpp
	terrain.cpp
    types.cpp
	world.cpp
	universe.cpp
)

# TODO: remove once migration is done.
add_subdirectory(old/)
