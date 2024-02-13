// Copyright 2018-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>


namespace openage {

namespace renderer {
class RenderFactory;
}

namespace gamestate {
class GameState;

/**
 * Entity for managing "physical" things (units, buildings) inside
 * the game.
 */
class World {
public:
	/**
	 * Create a new world.
	 *
	 * @param state State of the game.
	 */
	World(const std::shared_ptr<GameState> &state);
	~World() = default;

	/**
	 * Attach a renderer which enables graphical display options for all ingame entities.
	 *
	 * @param render_factory Factory for creating connector objects for gamestate->renderer
	 *                       communication.
	 */
	void attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory);

private:
	/**
	 * State of the current game.
	 */
	std::shared_ptr<GameState> state;

	/**
	 * Factory for creating connector objects to the renderer which make game entities displayable.
	 */
	std::shared_ptr<renderer::RenderFactory> render_factory;
};

} // namespace gamestate
} // namespace openage
