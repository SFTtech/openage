// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "main.h"

#include <thread>
#include <version>

#include "cvar/cvar.h"
#include "engine/engine.h"
#include "event/time_loop.h"
#include "log/log.h"
#include "presenter/presenter.h"
#include "util/timer.h"

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

/*
 * This function is called from Cython, see main_cpp.pyx.
 *
 * This is the main entry point to the C++ part.
 */
int run_game(const main_arguments &args) {
	log::log(MSG(info)
	         << "launching engine with "
	         << args.root_path
	         << " and fps limit "
	         << args.fps_limit);

	// TODO: store args.fps_limit and args.gl_debug as default in the cvar system.

	util::Timer timer;
	timer.start();

	// read and apply the configuration files
	auto cvar_manager = std::make_shared<cvar::CVarManager>(args.root_path["cfg"]);
	cvar_manager->load_all();

	// TODO: select run_mode by launch argument
	openage::engine::GameSimulation::mode run_mode = engine::GameSimulation::mode::FULL;

	// TODO: Order of initializing presenter, simulation and engine is not intuitive
	//       ideally it should be presenter->engine->simulation
	auto time_loop = std::make_shared<event::TimeLoop>();

	auto engine = std::make_shared<engine::GameSimulation>(run_mode, args.root_path, cvar_manager, time_loop);
	auto presenter = std::make_shared<presenter::Presenter>(args.root_path, engine, time_loop);

	std::jthread event_loop_thread([&]() {
		time_loop->run();

		time_loop.reset();
	});
	std::jthread engine_thread([&]() {
		engine->run();

		engine.reset();
	});
	std::jthread presenter_thread([&]() {
		presenter->run();

		// Make sure that the presenter gets destructed in the same thread
		// otherwise OpenGL complains about missing contexts
		presenter.reset();
	});

	return 0;
}

} // namespace openage
