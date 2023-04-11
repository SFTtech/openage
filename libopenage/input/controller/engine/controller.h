// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <unordered_set>

#include "curve/discrete.h"
#include "input/event.h"

namespace openage {

namespace engine {
class Engine;
}

namespace event {
class Simulation;
}

namespace input::engine {

class BindingContext;

/**
 * Interface for game controllers.
 *
 * Controllers handle inputs from outside of a game (e.g. GUI, AI, scripts, ...)
 * and pass the resulting events to game entities. They also act as a form of
 * access control for using in-game functionality of game entities.
 *
 * TODO: Connection to engine
 */
class Controller {
public:
	Controller(const std::unordered_set<size_t> &controlled_factions,
	           size_t active_faction_id);

	~Controller() = default;

	/**
     * Switch the actively controlled faction by the controller.
     * The ID must be in the list of controlled factions.
     *
     * @param faction_id ID of the new active faction.
     */
	void set_control(size_t faction_id);

	/**
     * Get the ID of the faction actively controlled by the controller.
     *
     * @return ID of the active faction.
     */
	size_t get_controlled();

	/**
     * Process an input event from the input manager.
     *
     * @param ev_args Input event and arguments.
     * @param ctx Binding context for looking up the event transformation.
     *
     * @return true if the event is accepted, else false.
     */
	bool process(const event_arguments &ev_args, const std::shared_ptr<BindingContext> &ctx);

private:
	/**
     * List of factions controllable by this controller.
     */
	std::unordered_set<size_t> controlled_factions;

	/**
     * ID of the currently active faction.
     */
	size_t active_faction_id;

	/**
     * Queue for gamestate events generated from inputs.
     */
	std::vector<std::shared_ptr<event::Event>> outqueue;
};

/**
 * Setup default controller action bindings:
 *
 * - Mouse click: Create game entity.
 *
 * @param ctx Binding context the actions are added to.
 * @param simulation Simulation with event loop.
 */
void setup_defaults(const std::shared_ptr<BindingContext> &ctx,
                    const std::shared_ptr<event::Simulation> &simulation,
                    const std::shared_ptr<openage::engine::Engine> &engine);

} // namespace input::engine
} // namespace openage