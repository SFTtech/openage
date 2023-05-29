// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "main.h"

#include <thread>
#include <version>

#include "cvar/cvar.h"
#include "engine/engine.h"
#include "event/time_loop.h"
#include "gamestate/simulation.h"
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
	// TODO: store args.fps_limit and args.gl_debug as default in the cvar system.

	util::Timer timer;
	timer.start();

	// read and apply the configuration files
	auto cvar_manager = std::make_shared<cvar::CVarManager>(args.root_path["cfg"]);
	cvar_manager->load_all();

	// TODO: select run_mode by launch argument
	openage::engine::Engine::mode run_mode = openage::engine::Engine::mode::FULL;
	openage::engine::Engine engine{run_mode, args.root_path};

	engine.loop();

	return 0;
}

} // namespace openage
