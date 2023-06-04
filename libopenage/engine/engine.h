// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <thread>
#include <vector>

#include "util/path.h"

// TODO: Remove custom jthread definition when clang/libc++ finally supports it
#if __llvm__
#if !__cpp_lib_jthread
namespace std {
class jthread : public thread {
public:
	using thread::thread; // needed constructors
	~jthread() {
		this->join();
	}
};
} // namespace std
#endif
#endif


namespace openage {

namespace cvar {
class CVarManager;
} // namespace cvar

namespace event {
class TimeLoop;
} // namespace event

namespace gamestate {
class GameSimulation;
} // namespace gamestate

namespace presenter {
class Presenter;
} // namespace presenter


namespace engine {

/**
 * Encapsulates all subcomponents needed for a run.
 */
class Engine {
public:
	enum class mode {
		LEGACY,
		HEADLESS,
		FULL,
	};

	/**
	 * Create the engine instance for this run.
     *
     * @param mode The run mode to use.
     * @param root_dir openage root directory.
	 */
	Engine(mode mode,
	       const util::Path &root_dir);

	// engine should not be copied or moved
	Engine(const Engine &) = delete;
	Engine &operator=(const Engine &) = delete;
	Engine(Engine &&) = delete;
	Engine &operator=(Engine &&) = delete;
	~Engine() = default;


	/**
     * Run the main loop.
     */
	void loop();

	/**
	 * current simulation state variable.
	 * to be set to false to stop the simulation loop.
	 */
	bool running;

private:
	/**
     * The run mode to use.
     */
	mode run_mode;

	/**
     * openage root directory.
     */
	util::Path root_dir;

	/**
     * The threads used by the engine.
     */
	std::vector<std::jthread> threads;

	/**
     * Environment variables.
     */
	std::shared_ptr<cvar::CVarManager> cvar_manager;

	/**
     * Controls and update the clock for time-based measurements.
     */
	std::shared_ptr<event::TimeLoop> time_loop;

	/**
     * Gameplay simulation.
     */
	std::shared_ptr<gamestate::GameSimulation> simulation;

	/**
     * Video/audio/input management. Can be nullptr in headless mode.
     */
	std::shared_ptr<presenter::Presenter> presenter;
};

} // namespace engine
} // namespace openage
