add_sources(libopenage
    definitions.cpp
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

add_subdirectory(component/)
add_subdirectory(event/)

# TODO: remove once migration is done.
add_subdirectory(old/)
