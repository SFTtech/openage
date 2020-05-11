// Copyright 2020-2020 the openage authors. See copying.md for legal info.

#include "versions.h"

#ifdef __linux__
#include <gnu/libc-version.h>
#endif
#ifdef __MINGW32__
#define SDL_MAIN_HANDLED
#endif

#include <eigen3/Eigen/Dense>
#include <epoxy/gl.h>
#include <harfbuzz/hb.h>
#include <opus_defines.h>
#include <QtGlobal>
#include <SDL2/SDL.h>
#include <sstream>

#include "versions/compiletime.h"
#include "../util/strings.h"

namespace openage::versions {

int main(int /*argc*/, char /**argv[]*/) { return 0;}

std::map<std::string, std::string> get_version_numbers() {

	std::map<std::string, std::string> version_numbers;

	// SDL runtime version number
	SDL_version sdl_runtime_version;
	SDL_GetVersion(&sdl_runtime_version);
	version_numbers.emplace("SDL-runtime", util::sformat("%d.%d.%d",
	                                                     sdl_runtime_version.major,
	                                                     sdl_runtime_version.minor,
	                                                     sdl_runtime_version.patch));

	// Eigen compiletime version number
	version_numbers.emplace("Eigen", util::sformat("%d.%d.%d",
	                                               EIGEN_WORLD_VERSION,
	                                               EIGEN_MAJOR_VERSION,
	                                               EIGEN_MINOR_VERSION));

	// Harfbuzz compiletime version number
	version_numbers.emplace("Harfbuzz", util::sformat("%d.%d.%d",
	                                                  HB_VERSION_MAJOR,
	                                                  HB_VERSION_MINOR,
	                                                  HB_VERSION_MICRO));

	// Add Qt version number
	version_numbers.emplace("Qt", QT_VERSION_STR);

	// Add nyan version number
	version_numbers.emplace("nyan", nyan_version);

	// Add OpenGL version number
	// TODO: set the same SDL_GL_CONTEXT_MAJOR_VERSION as the real renderer
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		version_numbers.emplace("OpenGL", SDL_GetError());
	}
	else {
		SDL_Window *window = SDL_CreateWindow("Query OpenGL version",
		                                      0, 0, 640, 480,
		                                      SDL_WINDOW_OPENGL|SDL_WINDOW_HIDDEN);
		if (window != nullptr) {
			SDL_GLContext glcontext = SDL_GL_CreateContext(window);
			if (glcontext != nullptr) {
				version_numbers.emplace("OpenGL",
				                        reinterpret_cast<const char *>(glGetString(GL_VERSION)));
				SDL_GL_DeleteContext(glcontext);
			}
			SDL_DestroyWindow(window);
		}
		SDL_Quit();
	}

	// Add Opus version number
	std::string opus_version = opus_get_version_string();
	version_numbers.emplace("Opus", opus_version.substr(opus_version.find(' ') + 1));

#if defined(__linux__)
	// Add libc version number if not MacOSX
	version_numbers.emplace("libc-runtime", gnu_get_libc_version());

	version_numbers.emplace("libc-compile", util::sformat("%d.%d",
	                                                      __GLIBC__,
	                                                      __GLIBC_MINOR__));
#endif

#ifdef __APPLE__
	version_numbers.emplace("libc-runtime", "Apple");
#endif

#if defined(_WIN32) & !defined(__MINGW32__)
	version_numbers.emplace("libc-runtime", "Windows");
#endif

#if defined(__MINGW32__)
    version_numbers.emplace("glibcxx-compile", util::sformat("%d",
                                                            __GLIBCXX__));
#endif

	return version_numbers;
}

} // namespace openage::versions
