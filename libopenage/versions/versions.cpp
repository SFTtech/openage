// Copyright 2020-2024 the openage authors. See copying.md for legal info.

#include "versions.h"

#ifdef __linux__
	#include <gnu/libc-version.h>
#endif

#include <eigen3/Eigen/Dense>
#include <epoxy/gl.h>
#include <harfbuzz/hb.h>
#include <opus_defines.h>
#include <QtGlobal>
#include <sstream>

#include "../util/strings.h"
#include "versions/compiletime.h"

namespace openage::versions {

std::map<std::string, std::string> get_version_numbers() {
	std::map<std::string, std::string> version_numbers;

	// Eigen compiletime version number
	version_numbers.emplace("Eigen", util::sformat("%d.%d.%d", EIGEN_WORLD_VERSION, EIGEN_MAJOR_VERSION, EIGEN_MINOR_VERSION));

	// Harfbuzz compiletime version number
	version_numbers.emplace("Harfbuzz", util::sformat("%d.%d.%d", HB_VERSION_MAJOR, HB_VERSION_MINOR, HB_VERSION_MICRO));

	// Add Qt version number
	version_numbers.emplace("Qt", QT_VERSION_STR);

	// Add nyan version number
	version_numbers.emplace("nyan", nyan_version);

	// Add Opus version number
	std::string opus_version = opus_get_version_string();
	version_numbers.emplace("Opus", opus_version.substr(opus_version.find(' ') + 1));

	// TODO: Add OpenGL version number

#ifdef __linux__
	// Add libc version number if not MacOSX
	version_numbers.emplace("libc-runtime", gnu_get_libc_version());

	version_numbers.emplace("libc-compile", util::sformat("%d.%d", __GLIBC__, __GLIBC_MINOR__));
#endif

#ifdef __APPLE__
	version_numbers.emplace("libc-runtime", "Apple");
#endif

#ifdef __WINDOWS__
	version_numbers.emplace("libc-runtime", "Windows");
#endif

	return version_numbers;
}

} // namespace openage::versions
