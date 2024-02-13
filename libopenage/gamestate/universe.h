// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

namespace openage {

namespace renderer {
class RenderFactory;
}

namespace gamestate {
class GameState;
class Terrain;
class World;

/**
 * Entity for managing the "physical" game world entities (units, buildings, etc.) as well as
 * conceptual entities (players, resources, ...).
 *
 * TODO: Remove Universe and other subclasses.
 */
class Universe {
public:
	/**
	 * Create a new universe.
	 *
	 * @param state State of the game.
	 */
	Universe(const std::shared_ptr<GameState> &state);
	~Universe() = default;

	/**
	 * Get the object controlling entities in the game world (i.e. units, buildings, etc.).
	 *
	 * @return World object.
	 */
	std::shared_ptr<World> get_world();

	/**
	 * Get the object controlling the terrain.
	 *
	 * @return Terrain object.
	 */
	std::shared_ptr<Terrain> get_terrain();

	/**
	 * Attach a renderer which enables graphical display options for all ingame entities.
	 *
	 * @param render_factory Factory for creating connector objects for gamestate->renderer
	 *                       communication.
	 */
	void attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory);

private:
	/**
	 * Manages "physical" entities inside the game world (units, buildings, ambient, ...),
	 * excluding terrain.
	 */
	std::shared_ptr<World> world;

	/**
	 * Manages the terrain.
	 */
	std::shared_ptr<Terrain> terrain;

	/**
	 * Factory for creating connector objects to the renderer which make game entities displayable.
	 */
	std::shared_ptr<renderer::RenderFactory> render_factory;
};

} // namespace gamestate
} // namespace openage
