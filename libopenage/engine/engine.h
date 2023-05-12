// Copyright 2013-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <shared_mutex>

#include "util/path.h"

namespace openage {

namespace cvar {
class CVarManager;
}

namespace event {
class EventLoop;
class TimeLoop;
} // namespace event

namespace gamestate {
class EntityFactory;
class Game;

namespace event {
class Spawner;
} // namespace event

} // namespace gamestate

namespace renderer {
class RenderFactory;
}

namespace engine {


/**
 * Gameplay subsystem of the engine.
 *
 * Manages the state of the game world and the event loop.
 */
class Engine final {
public:
	enum class mode {
		LEGACY,
		HEADLESS,
		FULL,
	};

	/**
	 * engine initialization method.
	 * starts the engine subsystems depending on the requested run mode.
	 */
	Engine(mode mode,
	       const util::Path &root_dir,
	       const std::shared_ptr<cvar::CVarManager> &cvar_manager,
	       const std::shared_ptr<event::TimeLoop> time_loop);

	/**
	 * engine copy constructor.
	 */
	Engine(const Engine &copy) = delete;

	/**
	 * engine assignment operator.
	 */
	Engine &operator=(const Engine &copy) = delete;

	/**
	 * engine move constructor.
	 */
	Engine(Engine &&other) = delete;

	/**
	 * engine move operator.
	 */
	Engine &operator=(Engine &&other) = delete;

	~Engine() = default;

	/**
	 * Run the engine loop.
	 */
	void run();

	/**
	 * Start the engine loop.
	 */
	void start();

	/**
	 * enqueues the stop of the main loop.
	 */
	void stop();

	/**
	 * return the data directory where the engine was started from.
	 */
	const util::Path &get_root_dir();

	/**
	 * Get this engine's cvar manager.
     *
     * @return CVarManager instance.
	 */
	const std::shared_ptr<cvar::CVarManager> get_cvar_manager();

	/**
     * Get the game running in the engine.
     *
     * @return Game instance.
     */
	const std::shared_ptr<gamestate::Game> get_game();

	/**
     * Get the event loop for the gamestate.
     *
     * @return Event loop.
     */
	const std::shared_ptr<event::EventLoop> get_event_loop();

	/**
     * Get the spawner event entity.
     *
     * TODO: Move somewhere else or remove.
     *
     * @return Spawner for entity creation.
     */
	const std::shared_ptr<gamestate::event::Spawner> get_spawner();

	/**
	 * Attach a renderer to the engine.
	 *
	 * @param factory Factory for creating render entities.
	 */
	void attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory);

	/**
	 * current engine state variable.
	 * to be set to false to stop the engine loop.
	 */
	bool running;

private:
	/**
     * Initialize event handlers.
     */
	void init_event_handlers();

	/**
	 * Run-mode of the engine, this determines the basic modules to be loaded.
	 */
	mode run_mode;

	/**
	 * The engine root directory.
	 * Uses the openage fslike path abstraction that can mount paths into one.
	 *
	 * This means that this path does simulataneously lead to global assets,
	 * home-folder-assets, settings, and basically the whole filesystem access.
	 *
	 * TODO: move this to a settings class, which then also hosts cvar and the options system.
	 */
	util::Path root_dir;

	/**
	 * the engine's cvar manager.
	 */
	std::shared_ptr<cvar::CVarManager> cvar_manager;

	/**
	 * Event time_loop for creating and sending events.
	 */
	std::shared_ptr<event::TimeLoop> time_loop;

	/**
     * Event loop for processing events in the game.
     */
	std::shared_ptr<event::EventLoop> event_loop;

	/**
     * Factory for creating game entities.
     */
	std::shared_ptr<gamestate::EntityFactory> entity_factory;

	// TODO: move somewhere sensible or remove
	std::shared_ptr<gamestate::event::Spawner> spawner;

	// TODO: The game run by the engine
	std::shared_ptr<gamestate::Game> game;

	std::shared_mutex mutex;
};

} // namespace engine
} // namespace openage
